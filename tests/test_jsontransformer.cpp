// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/ILogTransformerBuilder.hpp"
#include "Herald/ILogWriterBuilder.hpp"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "Herald/Logger.hpp"
#include "gtest/gtest.h"

TEST(JsonTransformer, basicLog)
{
	auto jsonTransformerBuilder = Herald::createJsonLogTransformerBuilder();
	auto jsonTransformer = jsonTransformerBuilder->attachLogWriterCallback([](const std::string & msg) { std::cerr << msg << std::endl; })
	                           .addHeader("userId", "test user")
	                           .addHeader("hostName", "test machine")
	                           .build();

	testing::internal::CaptureStderr();

	Herald::log(*jsonTransformer, Herald::LogLevels::Analysis, "Analysis message");

	// expecting well formed json in the output
	std::string output = testing::internal::GetCapturedStderr();

	// expecting a json object with a log level and message
	EXPECT_TRUE(output.find("{") != std::string::npos);
	EXPECT_TRUE(output.find("}") != std::string::npos);
	EXPECT_TRUE(output.find("\"level\": \"Analysis\"") != std::string::npos);
	EXPECT_TRUE(output.find("\"message\": \"Analysis message\"") != std::string::npos);
	EXPECT_TRUE(output.find("\"userId\": \"test user\"") != std::string::npos);
	EXPECT_TRUE(output.find("\"hostName\": \"test machine\"") != std::string::npos);
}

TEST(JsonTransformer, directLevelDisabledReturns)
{
	auto jsonTransformerBuilder = Herald::createJsonLogTransformerBuilder();
	auto jsonTransformer        = jsonTransformerBuilder->build();

	auto levels = Herald::getEnabledLogLevels();
	Herald::disableAllLogLevels();
	const std::string key("key");
	const std::string value("value");
	// just a coverage test, no output expected
	jsonTransformer->log(Herald::LogEntry(Herald::LogLevels::Analysis, "Analysis message", key, value));

	Herald::setLogLevels(levels);
}
