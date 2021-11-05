#include "Herald/Herald.hpp"
#include <atomic>
#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stdarg.h>
#include <thread>
#include <vector>

namespace Herald
{
#ifdef _DEBUG
#define LOCK()                                                                 \
	if (nullptr == _context) {                                                 \
		std::cerr << "Herald::install() needs to be called first. Aborting!";  \
		abort();                                                               \
	}                                                                          \
	std::lock_guard<std::mutex> lock(_context->_loggerMutex);
#else
#define LOCK() std::lock_guard<std::mutex> lock(_context->_loggerMutex);
#endif // LOCK()
	void logWorker();
	struct Context {
		Context()
		    : _done(false)
		    , _loggerMutex()
		    , _logThreadConditionVariable()
		    , _logThreadConditionVariableMutex()
		    , _logQueue()
		    , _logQueueMutex()
		    , _logWorker(logWorker)
		{
		}
		~Context()
		{
			_done = true;
			_logThreadConditionVariable.notify_one();
			_logWorker.join();
		}
		std::atomic<bool> _done;
		// std::atomic<bool>       _workerReady;
		std::mutex              _loggerMutex;
		std::condition_variable _logThreadConditionVariable;
		std::mutex              _logThreadConditionVariableMutex;
		std::queue<std::pair<LogTypes, std::string>> _logQueue;
		std::mutex                                   _logQueueMutex;
		std::thread                                  _logWorker;
		std::map<std::string, std::string>           _jsonLogHeader;
	};

	Context * _context = 0;

	std::vector<void (*)(const std::string &)> _logMessageCallbacks;
	std::vector<void (*)(const std::string &)> _jsonLogMessageCallbacks;

	uint32_t _enabledLogTypes;
	bool     _shouldAbortOnFatal(true);

	void enableLogType(LogTypes logType)
	{
		LOCK();
		_enabledLogTypes |= static_cast<uint32_t>(logType);
	}

	void disableLogType(LogTypes logType)
	{
		LOCK()
		_enabledLogTypes = _enabledLogTypes & ~static_cast<uint32_t>(logType);
	}

	void enableAllLogTypes()
	{
		LOCK();
		_enabledLogTypes = 0xFFFFFFFF;
	}
	void clearConfiguration()
	{
		LOCK();
		_enabledLogTypes = 0;
		_logMessageCallbacks.clear();
		_jsonLogMessageCallbacks.clear();
	}

	void enableAbortOnFatal()
	{
		LOCK();
		_shouldAbortOnFatal = true;
	}
	void disableAbortOnFatal()
	{
		LOCK();
		_shouldAbortOnFatal = false;
	}

	bool     shouldAbortOnFatal() { return _shouldAbortOnFatal; }
	uint32_t getEnabledLogTypes()
	{
		LOCK();
		return _enabledLogTypes;
	}

	void addLogMessageCallback(void (*cb)(const std::string &))
	{
		LOCK();

		const auto & location = std::find(_logMessageCallbacks.begin(),
		                                  _logMessageCallbacks.end(), cb);
		if (location == _logMessageCallbacks.end())
			_logMessageCallbacks.push_back(cb);
	}

	void removeLogMessageCallback(void (*cb)(const std::string &))
	{
		LOCK();
		const auto & location = std::find(_logMessageCallbacks.begin(),
		                                  _logMessageCallbacks.end(), cb);
		if (location != _logMessageCallbacks.end()) {
			_logMessageCallbacks.erase(location);
		}
	}

	void logf(LogTypes logType, const char * const format, ...)
	{
		if (!(getEnabledLogTypes() & static_cast<uint32_t>(logType)))
			return;

		const size_t bufferSize = 4096;
		char         buffer[bufferSize];
		va_list      args;
		va_start(args, format);
		vsnprintf(buffer, bufferSize, format, args);
		va_end(args);

		log(logType, buffer);
	}

	void LogInternal::log(LogTypes logType, const std::string & message)
	{
		{
			LOCK();

			if (!_logMessageCallbacks.empty() ||
			    !_jsonLogMessageCallbacks.empty()) {

				{
					std::lock_guard<std::mutex> queueLock(
					    _context->_logQueueMutex);
					_context->_logQueue.push(
					    std::make_pair(logType, std::string(message)));
				}
				_context->_logThreadConditionVariable.notify_one();
			}
		}
		if (logType == LogTypes::Fatal && shouldAbortOnFatal()) {
			Herald::remove();
			throw std::runtime_error(message);
		}
	}

	void logWorker()
	{
		while (!_context->_done) {
			std::unique_lock<std::mutex> l(
			    _context->_logThreadConditionVariableMutex);

			_context->_logThreadConditionVariable.wait(l, []() {
				return (!_context->_logQueue.empty() ||
				        _context->_done == true);
			});

			LogTypes    logType;
			std::string message;

			{
				std::lock_guard<std::mutex> lg(_context->_logQueueMutex);
				if (_context->_logQueue.empty())
					continue;
				const auto & entry = _context->_logQueue.front();
				logType            = entry.first;
				message            = entry.second;
				_context->_logQueue.pop();
			}

			if (!_logMessageCallbacks.empty()) {
				std::stringstream formatted;
				formatted << message << std::endl;
				for (const auto & logSync : _logMessageCallbacks) {
					logSync(formatted.str());
				}
			}
		}
	}

	void install()
	{
		if (nullptr != _context) {
			throw std::runtime_error("install invoked twice");
		}
		_context = new Context();
	}

	void remove()
	{
		delete _context;
		_context = nullptr;
	}

	void setJsonLogHeader(const std::string & key, const std::string & value)
	{
		if (nullptr == _context)
			return;

		_context->_jsonLogHeader[key] = value;
	}

	void removeJsonLogHeader(const std::string & key)
	{
		if (nullptr == _context)
			return;
		LOCK();
		_context->_jsonLogHeader.erase(key);
	}

	void clearJsonLogHeaders()
	{
		if (nullptr == _context)
			return;

		LOCK();
		_context->_jsonLogHeader.clear();
	}

	std::map<std::string, std::string> getJsonLogHeaders()
	{
		std::map<std::string, std::string> result;

		if (nullptr == _context)
			return result;

		{
			LOCK();
			result = _context->_jsonLogHeader;
		}
		return result;
	}
} // namespace Herald
