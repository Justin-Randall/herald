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

	Herald::clearConfiguration();
	EXPECT_EQ(Herald::getEnabledLogTypes(), 0);
}
