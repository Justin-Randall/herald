// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/BaseLogTransformer.hpp"
#include "Herald/Logger.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

class StringLogTransformer : public Herald::BaseLogTransformer
{
  public:
	StringLogTransformer() = default;
	virtual void log(const Herald::LogEntry & entry) override { logString = entry.message; }
	std::string  logString;
};

TEST(Herald, LogLevels)
{
	// create a basic string log transformer and verify various combinations of log levels
	StringLogTransformer logTransformer;

	Herald::disableAllLogLevels();

	Herald::log(logTransformer, Herald::LogLevels::Analysis, "Analysis message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Trace, "Trace message", "testKey", "testValue");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Debug, "Debug message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Info, "Info message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Warning, "Warning message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Error, "Error message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::log(logTransformer, Herald::LogLevels::Fatal, "Fatal message");
	EXPECT_EQ(logTransformer.logString, "");

	Herald::enableAllLogLevels();

	Herald::log(logTransformer, Herald::LogLevels::Analysis, "Analysis message");
	EXPECT_EQ(logTransformer.logString, "Analysis message");

	Herald::log(logTransformer, Herald::LogLevels::Trace, "Trace message", "testKey", "testValue");
	EXPECT_EQ(logTransformer.logString, "Trace message");

	Herald::log(logTransformer, Herald::LogLevels::Debug, "Debug message");
	EXPECT_EQ(logTransformer.logString, "Debug message");

	Herald::log(logTransformer, Herald::LogLevels::Info, "Info message");
	EXPECT_EQ(logTransformer.logString, "Info message");

	Herald::log(logTransformer, Herald::LogLevels::Warning, "Warning message");
	EXPECT_EQ(logTransformer.logString, "Warning message");

	Herald::log(logTransformer, Herald::LogLevels::Error, "Error message");
	EXPECT_EQ(logTransformer.logString, "Error message");

	Herald::log(logTransformer, Herald::LogLevels::Fatal, "Fatal message");
	EXPECT_EQ(logTransformer.logString, "Fatal message");

	// clear them all, enable one by one and verify they are all logged
	Herald::disableAllLogLevels();

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Analysis, "Analysis message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Analysis);
	Herald::log(logTransformer, Herald::LogLevels::Analysis, "Analysis message");
	EXPECT_EQ(logTransformer.logString, "Analysis message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Trace, "Trace message", "testKey", "testValue");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Trace);
	Herald::log(logTransformer, Herald::LogLevels::Trace, "Trace message", "testKey", "testValue");
	EXPECT_EQ(logTransformer.logString, "Trace message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Debug, "Debug message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Debug);
	Herald::log(logTransformer, Herald::LogLevels::Debug, "Debug message");
	EXPECT_EQ(logTransformer.logString, "Debug message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Info, "Info message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Info);
	Herald::log(logTransformer, Herald::LogLevels::Info, "Info message");
	EXPECT_EQ(logTransformer.logString, "Info message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Warning, "Warning message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Warning);
	Herald::log(logTransformer, Herald::LogLevels::Warning, "Warning message");
	EXPECT_EQ(logTransformer.logString, "Warning message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Error, "Error message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Error);
	Herald::log(logTransformer, Herald::LogLevels::Error, "Error message");
	EXPECT_EQ(logTransformer.logString, "Error message");

	logTransformer.logString.clear();
	Herald::log(logTransformer, Herald::LogLevels::Fatal, "Fatal message");
	EXPECT_EQ(logTransformer.logString, "");
	Herald::enableLogLevel(Herald::LogLevels::Fatal);
	Herald::log(logTransformer, Herald::LogLevels::Fatal, "Fatal message");
	EXPECT_EQ(logTransformer.logString, "Fatal message");

	// lastly, an event, which SHOULD show up
	Herald::event(logTransformer, "Event message");
	EXPECT_EQ(logTransformer.logString, "Event message");
}

#if 0
TEST(Herald, Configuration)
{
	// Herald::install();
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	EXPECT_EQ(Herald::getEnabledLogTypes(c), 0);

	Herald::enableLogType(c, Herald::LogTypes::Analysis);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Analysis));

	Herald::enableLogType(c, Herald::LogTypes::Trace);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Trace));

	Herald::enableLogType(c, Herald::LogTypes::Debug);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Debug));

	Herald::enableLogType(c, Herald::LogTypes::Info);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Info));

	Herald::enableLogType(c, Herald::LogTypes::Warning);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Warning));

	Herald::enableLogType(c, Herald::LogTypes::Error);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Error));

	Herald::enableLogType(c, Herald::LogTypes::Fatal);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Fatal));

	Herald::disableLogType(c, Herald::LogTypes::Analysis);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Analysis));

	Herald::disableLogType(c, Herald::LogTypes::Trace);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Trace));

	Herald::disableLogType(c, Herald::LogTypes::Debug);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Debug));

	Herald::disableLogType(c, Herald::LogTypes::Info);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Info));

	Herald::disableLogType(c, Herald::LogTypes::Warning);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Warning));

	Herald::disableLogType(c, Herald::LogTypes::Error);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Error));

	Herald::disableLogType(c, Herald::LogTypes::Fatal);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Fatal));

	EXPECT_EQ(Herald::getEnabledLogTypes(c), 0);

	Herald::enableAllLogTypes(c);
	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Analysis));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Trace));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Debug));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Info));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Warning));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Error));

	EXPECT_TRUE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Fatal));

	Herald::clearConfiguration(c);
	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Analysis));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Trace));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Debug));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Info));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Warning));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Error));

	EXPECT_FALSE(Herald::getEnabledLogTypes(c) & static_cast<uint32_t>(Herald::LogTypes::Fatal));
	Herald::destroyContext(ctx);
}

namespace
{
	std::atomic<bool>       testCallbackInvoked;
	const std::string       testCallbackMessage("testing");
	std::mutex              conditionLock;
	std::condition_variable condition;
	std::mutex              testLock;
} // namespace

void testCallback(const std::string &)
{
	testCallbackInvoked = true;
	condition.notify_one();
}

TEST(Herald, Callbacks)
{
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	testCallbackInvoked = false;

	Herald::enableLogType(c, Herald::LogTypes::Debug);

	Herald::addLogMessageCallback(c, &testCallback);

	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::log(c, Herald::LogTypes::Debug, testCallbackMessage);
		condition.wait(l);
		EXPECT_TRUE(testCallbackInvoked);
	}
	testCallbackInvoked = false;

	Herald::removeLogMessageCallback(c, &testCallback);
	Herald::log(c, Herald::LogTypes::Debug, testCallbackMessage);
	EXPECT_FALSE(testCallbackInvoked);

	Herald::destroyContext(ctx);
}

std::string lastLogCallbackMessage;

void logCallback(const std::string & message)
{
	lastLogCallbackMessage = message;
	condition.notify_one();
}

TEST(Herald, Logging)
{
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	Herald::clearConfiguration(c);

	Herald::enableAllLogTypes(c);
	Herald::addLogMessageCallback(c, &logCallback);

	const char * const stringLiteral = "string literal";

	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::log(c, Herald::LogTypes::Debug, stringLiteral);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find(stringLiteral) != std::string::npos);
	}

	{
		std::unique_lock<std::mutex> l(conditionLock);
		const std::string            stdString("std::string");
		Herald::log(c, Herald::LogTypes::Debug, stdString);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find(stdString) != std::string::npos);
	}
	int answer = 42;
	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::logf(c, Herald::LogTypes::Debug, "The answer is %d", answer);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find("The answer is 42") != std::string::npos);
	}

	Herald::destroyContext(ctx);
}

TEST(Herald, FatalErrorHandlingThrows)
{
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	Herald::enableAbortOnFatal(c);
	Herald::enableLogType(c, Herald::LogTypes::Fatal);
	bool fatalErrorThrows = false;
	try {
		Herald::log(c, Herald::LogTypes::Fatal, "Fatal!");
	} catch (std::runtime_error) {
		fatalErrorThrows = true;
	}
	EXPECT_TRUE(fatalErrorThrows);

	Herald::destroyContext(ctx);
}

TEST(Herald, FatalErrorHandlingNoThrow)
{
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	Herald::disableAbortOnFatal(c);
	Herald::enableLogType(c, Herald::LogTypes::Fatal);
	bool fatalErrorThrows = false;

	fatalErrorThrows = false;
	Herald::log(c, Herald::LogTypes::Fatal, "Fatal!");
	EXPECT_FALSE(fatalErrorThrows);

	Herald::destroyContext(ctx);
}

TEST(Herald, LogFormatDoesNotLogWhenLogTypeDisabled)
{
	auto ctx = Herald::createContext();
	EXPECT_NE(nullptr, ctx);

	Herald::Context & c = *ctx;

	Herald::disableLogType(c, Herald::LogTypes::Debug);
	Herald::logf(c, Herald::LogTypes::Debug, "LogFormat Ignored!");

	Herald::destroyContext(ctx);
}
#endif // 0
