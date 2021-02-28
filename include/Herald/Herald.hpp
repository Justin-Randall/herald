#ifndef _INCLUDED_Logger_HPP
#define _INCLUDED_Logger_HPP

#include <iostream>
#include <stdint.h>

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

	class LogInternal
	{
	  private:
		LogInternal() = delete;
		friend void log(const LogTypes, const char *);
		static void log(const LogTypes, const char *);
	};

	void clearConfiguration();

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
		if (!(getEnabledLogTypes() & static_cast<uint32_t>(logType)))
			return;

		LogInternal::log(logType, message);
	}

	inline void log(const LogTypes logType, const std::string & message)
	{
		if (!(getEnabledLogTypes() & static_cast<uint32_t>(logType)))
			return;

		log(logType, message.c_str());
	}

	void logf(const LogTypes logType, const char * const format, ...);

	void addLogMessageCallback(void (*cb)(const std::string &));
	void removeLogMessageCallback(void (*cb)(const std::string &));

} // namespace Herald
#endif //_INCLUDED_Logger_HPP
