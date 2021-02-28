#include "Herald/Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <map>
#include <mutex>
#include <sstream>
#include <stdarg.h>
#include <vector>

namespace Logger
{
	std::mutex _loggerMutex;

	std::vector<void (*)(const std::string &)> _logMessageCallbacks;
	std::vector<void (*)(const std::string &)> _jsonLogMessageCallbacks;
	const std::map<LogTypes, std::string>      logTypeNames = {
        {(LogTypes::Analysis), "Analysis"}, {(LogTypes::Debug), "Debug"},
        {(LogTypes::Info), "Info"},         {(LogTypes::Warning), "Warning"},
        {(LogTypes::Error), "Error"},       {(LogTypes::Fatal), "Fatal"},
        {(LogTypes::Trace), "Trace"}};

	uint32_t _enabledLogTypes;
	bool     _shouldAbortOnFatal(true);

	void enableLogType(LogTypes logType)
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_enabledLogTypes |= static_cast<uint32_t>(logType);
	}

	void disableLogType(LogTypes logType)
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_enabledLogTypes = _enabledLogTypes & ~static_cast<uint32_t>(logType);
	}

	void enableAllLogTypes()
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_enabledLogTypes = 0xFFFFFFFF;
	}
	void clearConfiguration()
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_enabledLogTypes = 0;
	}
	void enableAbortOnFatal()
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_shouldAbortOnFatal = true;
	}
	void disableAbortOnFatal()
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		_shouldAbortOnFatal = false;
	}
	uint32_t getEnabledLogTypes()
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);
		return _enabledLogTypes;
	}

	void addLogMessageCallback(void (*cb)(const std::string &))
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);

		const auto & location = std::find(_logMessageCallbacks.begin(),
		                                  _logMessageCallbacks.end(), cb);
		if (location == _logMessageCallbacks.end())
			_logMessageCallbacks.push_back(cb);
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

	void LogInternal::log(LogTypes logType, const char * const message)
	{
		std::lock_guard<std::mutex> lock(_loggerMutex);

		if (_logMessageCallbacks.empty() && _jsonLogMessageCallbacks.empty())
			return;

		auto        currentChronoTime = std::chrono::system_clock::now();
		std::time_t currentTime =
		    std::chrono::system_clock::to_time_t(currentChronoTime);

		std::chrono::milliseconds currentChronoMs =
		    std::chrono::duration_cast<std::chrono::milliseconds>(
		        currentChronoTime.time_since_epoch());

		std::stringstream timeStamp;
		timeStamp << std::put_time(std::localtime(&currentTime),
		                           "%Y-%m-%dT%H:%M:%S")
		          << "." << std::setfill('0') << std::setw(3)
		          << (currentChronoMs.count() % 1000)
		          << std::put_time(std::localtime(&currentTime), "%z");

		const auto  f = logTypeNames.find(logType);
		std::string logTypeName;
		if (f != logTypeNames.end()) {
			logTypeName = (*f).second;
		}

		if (!_logMessageCallbacks.empty()) {
			static const std::string separator(" : ");
			std::stringstream        formatted;
			formatted << timeStamp.str() << separator;
			if (logTypeName.length() > 0) {
				formatted << logTypeName << separator;
			}
			formatted << message << std::endl;
			for (const auto & logSync : _logMessageCallbacks) {
				logSync(formatted.str());
			}
		}

		if (logType == LogTypes::Fatal && _shouldAbortOnFatal) {
			abort();
		}
	}
} // namespace Logger
