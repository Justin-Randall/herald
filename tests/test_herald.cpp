#include "Herald/Logger.hpp"
#include "gtest/gtest.h"

TEST(Herald, Configuration)
{
	EXPECT_EQ(Logger::getEnabledLogTypes(), 0);

	Logger::enableLogType(Logger::LogTypes::Analysis);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Analysis));

	Logger::enableLogType(Logger::LogTypes::Trace);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Trace));

	Logger::enableLogType(Logger::LogTypes::Debug);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Debug));

	Logger::enableLogType(Logger::LogTypes::Info);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Info));

	Logger::enableLogType(Logger::LogTypes::Warning);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Warning));

	Logger::enableLogType(Logger::LogTypes::Error);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Error));

	Logger::enableLogType(Logger::LogTypes::Fatal);
	EXPECT_TRUE(Logger::getEnabledLogTypes() &
	            static_cast<uint32_t>(Logger::LogTypes::Fatal));

	Logger::clearConfiguration();
	EXPECT_EQ(Logger::getEnabledLogTypes(), 0);
}
