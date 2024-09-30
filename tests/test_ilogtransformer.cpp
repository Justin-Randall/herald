// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/ILogTransformerBuilder.hpp"
#include "Herald/ILogWriterBuilder.hpp"
#include "Herald/Logger.hpp"
#include "Herald/SimpleLogTransformerFactory.hpp"
#include "gtest/gtest.h"

TEST(ILogTransformer, logBuilderWithLambdaStdErr)
{
	auto logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();
	auto logTransformer =
	    logTransformerBuilder->attachLogWriterCallback([](const std::string & msg) { std::cerr << msg << std::endl; }).build();

	testing::internal::CaptureStderr();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message");

	std::string output = testing::internal::GetCapturedStderr();
	EXPECT_STREQ(output.c_str(), "[Analysis] Analysis message\n");
}

int  callCount = 0;
void logCallback(const std::string &) { ++callCount; }

TEST(ILogTransformer, addRemoveCallbacks)
{
	auto logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();
	auto logTransformer        = logTransformerBuilder->attachLogWriterCallback(logCallback).build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Should increment call count");

	EXPECT_EQ(callCount, 1);

	logTransformer->removeLogCallback(logCallback);
	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Should not increment call count");

	EXPECT_EQ(callCount, 1);
}

TEST(ILogTransformer, mixedTypeStructuredLogging)
{
	std::string callbackString;
	auto        logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();
	auto        logTransformer =
	    logTransformerBuilder->attachLogWriterCallback([&callbackString](const std::string & msg) { callbackString = msg; }).build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message", "key", "value", "key2", 42, 3.1415, -355);

	// the order of the key value pairs is not guaranteed so the output needs to contain all the pairs in any order in callbackString
	EXPECT_TRUE(callbackString.find("[Analysis] Analysis message [") != std::string::npos);
	EXPECT_TRUE(callbackString.find("key=value") != std::string::npos);
	EXPECT_TRUE(callbackString.find("key2=42") != std::string::npos);

	// converting a double to a string is not guaranteed to be exactly the same as the original value
	// perform the conversion here as well to compare the strings
	const std::string piStr = std::to_string(3.1415);
	EXPECT_TRUE(callbackString.find(piStr + "=-355") != std::string::npos);
}

TEST(ILogTransformer, addAndRemoveHeaders)
{
	auto logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();

	std::string callbackString;
	auto        logTransformer = logTransformerBuilder->addHeader("headerKey", "headerValue")
	                          .attachLogWriterCallback([&callbackString](const std::string & msg) { callbackString = msg; })
	                          .build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message");

	EXPECT_TRUE(callbackString.find("[headerKey=headerValue] [Analysis] Analysis message") != std::string::npos);

	logTransformer->removeHeader("headerKey");
	callbackString.clear();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message");

	EXPECT_TRUE(callbackString.find("[Analysis] Analysis message") != std::string::npos);
}

struct TestStringWriter {
	std::string logString;
	void        write(const std::string & msg) { logString = msg; }
};

TEST(iLogTransformer, memberFunctionCallback)
{
	auto             logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();
	TestStringWriter logWriter;

	// attach a member function callback for TestStringWriter::write from logWriter
	auto logTransformer =
	    logTransformerBuilder->attachLogWriterCallback([&logWriter](const std::string & msg) { logWriter.write(msg); }).build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message");

	EXPECT_TRUE(logWriter.logString.find("[Analysis] Analysis message") != std::string::npos);
}

TEST(iLogTransformer, disbaledLogLevelReturnsNoOp)
{
	auto logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();
	auto logTransformer        = logTransformerBuilder->build();

	auto levels = Herald::getEnabledLogLevels();
	Herald::disableAllLogLevels();

	// just a coverage test, no output expected
	logTransformer->log(Herald::LogEntry(Herald::LogLevels::Analysis, "Analysis message"));

	Herald::setLogLevels(levels);
}