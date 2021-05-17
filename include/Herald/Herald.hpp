#ifndef _INCLUDED_Logger_HPP
#define _INCLUDED_Logger_HPP

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdint.h>
#include <string>

// helper macros
#define LOG_TRACE(x) Logger::log(Logger::LogTypes::Trace, x);
#define LOG_INFO(x) Logger::log(Logger::LogTypes::Info, x);
#define LOG_DEBUG(x) Logger::log(Logger::LogTypes::Debug, x);
#define LOG_WARNING(x) Logger::log(Logger::LogTypes::Warning, x);
#define LOG_FATAL(x) Logger::log(Logger::LogTypes::Fatal, x);

#define LOG_TRACE_FORMAT(x, ...)                                               \
	Logger::logf(Logger::LogTypes::Trace, x, __VA_ARGS__);
#define LOG_INFO_FORMAT(x, ...)                                                \
	Logger::logf(Logger::LogTypes::Info, x, __VA_ARGS__);
#define LOG_DEBUG_FORMAT(x, ...)                                               \
	Logger::logf(Logger::LogTypes::Debug, x, __VA_ARGS__);
#define LOG_WARNING_FORMAT(x, ...)                                             \
	Logger::logf(Logger::LogTypes::Warning, x, __VA_ARGS__);
#define LOG_FATAL_FORMAT(x, ...)                                               \
	Logger::logf(Logger::LogTypes::Fatal, x, __VA_ARGS__);

namespace Herald
{
	enum struct LogTypes : uint32_t {
		Analysis = (1 << 0),
		Trace    = (1 << 1),
		Debug    = (1 << 2),
		Info     = (1 << 3),
		Warning  = (1 << 4),
		Error    = (1 << 5),
		Fatal    = (1 << 6)
	};

	const std::map<LogTypes, std::string> logTypeNames = {
	    {(LogTypes::Analysis), "Analysis"}, {(LogTypes::Debug), "Debug"},
	    {(LogTypes::Info), "Info"},         {(LogTypes::Warning), "Warning"},
	    {(LogTypes::Error), "Error"},       {(LogTypes::Fatal), "Fatal"},
	    {(LogTypes::Trace), "Trace"}};

	class LogInternal
	{
	  private:
		LogInternal() = delete;
		friend void log(const LogTypes, const std::string &);
		// template <typename ValueType, typename... JSONPairs>
		// friend void log_json(const LogTypes      logType,
		//                      const std::string & logMessage,
		//                      const std::string & variableName,
		//                      ValueType & variable, const JSONPairs &...
		//                      tuples);
		template <typename ValueType, typename... Args>
		friend void log_json(const LogTypes & logType, const std::string & msg,
		                     const std::string & key, const ValueType & value,
		                     const Args &... args);
		static void log(const LogTypes, const std::string &);
	};

	void     clearConfiguration();
	void     enableLogType(LogTypes logType);
	void     disableLogType(LogTypes logType);
	void     enableAllLogTypes();
	uint32_t getEnabledLogTypes();
	void     disableAbortOnFatal();
	void     enableAbortOnFatal();

	// Contains everything already (either json serialized, or just a raw
	// log message that needs to be encapsulated in a json log entry with a
	// timesteamp)
	inline void log(const LogTypes logType, const char * message)
	{
		const std::string msg(message);
		log(logType, msg);
	}

	inline std::string getTimeStamp()
	{
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
		return timeStamp.str();
	}

	inline void log(const LogTypes logType, const std::string & message)
	{
		if (!(getEnabledLogTypes() & static_cast<uint32_t>(logType)))
			return;

		const std::string timeStamp = getTimeStamp();
		const auto        f         = logTypeNames.find(logType);
		std::string       logTypeName;
		if (f != logTypeNames.end()) {
			logTypeName = (*f).second;
		}
		static const std::string separator(" : ");

		LogInternal::log(logType, timeStamp + separator + logTypeName +
		                              separator + message);
	}

	void logf(const LogTypes logType, const char * const format, ...);

	void addLogMessageCallback(void (*cb)(const std::string &));
	void removeLogMessageCallback(void (*cb)(const std::string &));
	void install();
	void remove();
	void setJsonLogHeader(const std::string & key, const std::string & value);
	void removeJsonLogHeader(const std::string & key);
	void clearJsonLogHeaders();

	// should be used internally. Returns copy by value for thread safety.
	std::map<std::string, std::string> getJsonLogHeaders();

	std::string getTimeStamp();

	// MainLogger
	// Helper for cases where log entries may come before or after
	// main(). Instantiate it in file scope for static initialization.
	struct MainLogger {
		MainLogger() { Herald::install(); }
		~MainLogger() { Herald::remove(); }
	};

} // namespace Herald
#endif //_INCLUDED_Logger_HPP
