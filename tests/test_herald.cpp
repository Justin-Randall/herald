#include "Herald/Herald.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

TEST(Herald, Configuration)
{
	Herald::install();
	EXPECT_EQ(Herald::getEnabledLogTypes(), 0);

	Herald::enableLogType(Herald::LogTypes::Analysis);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Analysis));

	Herald::enableLogType(Herald::LogTypes::Trace);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Trace));

	Herald::enableLogType(Herald::LogTypes::Debug);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Debug));

	Herald::enableLogType(Herald::LogTypes::Info);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Info));

	Herald::enableLogType(Herald::LogTypes::Warning);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Warning));

	Herald::enableLogType(Herald::LogTypes::Error);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Error));

	Herald::enableLogType(Herald::LogTypes::Fatal);
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Fatal));

	Herald::disableLogType(Herald::LogTypes::Analysis);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Analysis));

	Herald::disableLogType(Herald::LogTypes::Trace);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Trace));

	Herald::disableLogType(Herald::LogTypes::Debug);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Debug));

	Herald::disableLogType(Herald::LogTypes::Info);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Info));

	Herald::disableLogType(Herald::LogTypes::Warning);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Warning));

	Herald::disableLogType(Herald::LogTypes::Error);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Error));

	Herald::disableLogType(Herald::LogTypes::Fatal);
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Fatal));

	EXPECT_EQ(Herald::getEnabledLogTypes(), 0);

	Herald::enableAllLogTypes();
	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Analysis));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Trace));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Debug));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Info));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Warning));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Error));

	EXPECT_TRUE(Herald::getEnabledLogTypes() &
	            static_cast<uint32_t>(Herald::LogTypes::Fatal));

	Herald::clearConfiguration();
	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Analysis));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Trace));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Debug));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Info));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Warning));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Error));

	EXPECT_FALSE(Herald::getEnabledLogTypes() &
	             static_cast<uint32_t>(Herald::LogTypes::Fatal));
	Herald::remove();
}

namespace
{
	std::atomic<bool>       testCallbackInvoked;
	const std::string       testCallbackMessage("testing");
	std::mutex              conditionLock;
	std::condition_variable condition;
	std::mutex              testLock;
} // namespace

void testCallback(const std::string & msg)
{
	condition.notify_one();
	testCallbackInvoked = true;
}

TEST(Herald, Callbacks)
{
	Herald::install();

	testCallbackInvoked = false;

	Herald::enableLogType(Herald::LogTypes::Debug);

	Herald::addLogMessageCallback(&testCallback);

	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::log(Herald::LogTypes::Debug, testCallbackMessage);
		condition.wait(l);
		EXPECT_TRUE(testCallbackInvoked);
	}
	testCallbackInvoked = false;

	Herald::removeLogMessageCallback(&testCallback);
	Herald::log(Herald::LogTypes::Debug, testCallbackMessage);
	EXPECT_FALSE(testCallbackInvoked);

	Herald::remove();
}

std::string lastLogCallbackMessage;

void logCallback(const std::string & message)
{
	condition.notify_one();
	lastLogCallbackMessage = message;
}

TEST(Herald, Logging)
{
	Herald::install();
	Herald::clearConfiguration();

	Herald::enableAllLogTypes();
	Herald::addLogMessageCallback(&logCallback);

	const char * const stringLiteral = "string literal";

	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::log(Herald::LogTypes::Debug, stringLiteral);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find(stringLiteral) !=
		            std::string::npos);
	}

	{
		std::unique_lock<std::mutex> l(conditionLock);
		const std::string            stdString("std::string");
		Herald::log(Herald::LogTypes::Debug, stdString);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find(stdString) !=
		            std::string::npos);
	}
	int answer = 42;
	{
		std::unique_lock<std::mutex> l(conditionLock);
		Herald::logf(Herald::LogTypes::Debug, "The answer is %d", answer);
		condition.wait(l);
		EXPECT_TRUE(lastLogCallbackMessage.find("The answer is 42") !=
		            std::string::npos);
	}
	Herald::remove();
}

TEST(Herald, FatalErrorHandlingThrows)
{
	Herald::install();
	Herald::enableAbortOnFatal();
	Herald::enableLogType(Herald::LogTypes::Fatal);
	bool fatalErrorThrows = false;
	try {
		Herald::log(Herald::LogTypes::Fatal, "Fatal!");
	} catch (std::runtime_error) {
		fatalErrorThrows = true;
	}
	EXPECT_TRUE(fatalErrorThrows);
}

TEST(Herald, FatalErrorHandlingNoThrow)
{
	Herald::install();
	Herald::disableAbortOnFatal();
	Herald::enableLogType(Herald::LogTypes::Fatal);
	bool fatalErrorThrows = false;

	fatalErrorThrows = false;
	Herald::log(Herald::LogTypes::Fatal, "Fatal!");
	EXPECT_FALSE(fatalErrorThrows);
	Herald::remove();
}

TEST(Herald, LogFormatDoesNotLogWhenLogTypeDisabled)
{
	Herald::install();
	Herald::disableLogType(Herald::LogTypes::Debug);
	Herald::logf(Herald::LogTypes::Debug, "LogFormat Ignored!");
	Herald::remove();
}

// TEST(Herald, DoubleInstallThrows)
// {
// 	Herald::install();
// 	bool installThrows = false;
// 	try {
// 		Herald::install();
// 	} catch (std::runtime_error) {
// 		installThrows = true;
// 	}
// 	EXPECT_TRUE(installThrows);
// 	Herald::remove();
// }
