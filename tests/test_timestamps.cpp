// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/GetTimeStamp.hpp"
#include "gtest/gtest.h"

TEST(Herald, Timestamps)
{
	std::string defaultNow = Herald::getTimeStamp();
	EXPECT_FALSE(defaultNow.empty());
	// Expected format: "YYYY-MM-DDTHH:MM:SS.mmm+ZZZZ"
	// Check basic structure
	// Example: "2024-12-10T15:03:45.123+0000"
	// We expect at least the date, time, and milliseconds
	EXPECT_NE(std::string::npos, defaultNow.find("T")); // should contain 'T' separating date and time
	EXPECT_NE(std::string::npos, defaultNow.find(":")); // should have hours:minutes:seconds
	EXPECT_NE(std::string::npos, defaultNow.find(".")); // should have milliseconds

	std::chrono::system_clock::time_point epoch    = std::chrono::system_clock::time_point{} + std::chrono::milliseconds(123);
	std::string                           epochNow = Herald::getTimeStamp(epoch);
	// Check known parts of the string
	// The date should be 1970-01-01
	EXPECT_NE(std::string::npos, epochNow.find("1970-01-01"));
	EXPECT_NE(std::string::npos, epochNow.find(".123"));

	const auto converted = std::to_string(epoch);
	EXPECT_EQ(epochNow, converted);
}