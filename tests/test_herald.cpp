#include "Herald/Herald.hpp"
#include "gtest/gtest.h"

TEST(Herald, Configuration)
{
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
}

bool              testCallbackInvoked;
const std::string testCallbackMessage("testing");

void testCallback(const std::string & msg) { testCallbackInvoked = true; }

TEST(Herald, Callbacks)
{
	testCallbackInvoked = false;

	Herald::enableLogType(Herald::LogTypes::Debug);

	Herald::addLogMessageCallback(&testCallback);

	Herald::log(Herald::LogTypes::Debug, testCallbackMessage);
	EXPECT_TRUE(testCallbackInvoked);
	testCallbackInvoked = false;

	Herald::removeLogMessageCallback(&testCallback);
	Herald::log(Herald::LogTypes::Debug, testCallbackMessage);
	EXPECT_FALSE(testCallbackInvoked);
}

std::string lastLogCallbackMessage;

void logCallback(const std::string & message)
{
	lastLogCallbackMessage = message;
}

TEST(Herald, Logging) {
	Herald::clearConfiguration();

	Herald::enableAllLogTypes();
	Herald::addLogMessageCallback(&logCallback);

	const char * const stringLiteral = "string literal";

	Herald::log(Herald::LogTypes::Debug, stringLiteral);
	EXPECT_TRUE(lastLogCallbackMessage.find(stringLiteral) != std::string::npos);

	const std::string stdString("std::string");
	Herald::log(Herald::LogTypes::Debug, stdString);
	EXPECT_TRUE(lastLogCallbackMessage.find(stdString) != std::string::npos);

	int answer = 42;
	Herald::logf(Herald::LogTypes::Debug, "The answer is %d", answer);
	EXPECT_TRUE(lastLogCallbackMessage.find("The answer is 42") != std::string::npos);
}