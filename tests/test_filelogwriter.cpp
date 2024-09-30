#include "Herald/FileWriterFactory.hpp"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "Herald/Logger.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>

TEST(FileLogWriter, writeToStdOut)
{
	auto fileWriterBuilder = Herald::createFileWriterBuilder();
	auto fileWriter        = fileWriterBuilder->addConfigPair("filename", "stdout").build();

	testing::internal::CaptureStdout();

	fileWriter->write("Hello, world!");

	// ensure the worker thread has had a chance to write to the stream
	fileWriter.reset();

	std::string output = testing::internal::GetCapturedStdout();

	EXPECT_STREQ(output.c_str(), "Hello, world!\n");
}

TEST(FileLogWriter, writeToStdErr)
{
	auto fileWriterBuilder = Herald::createFileWriterBuilder();
	auto fileWriter        = fileWriterBuilder->addConfigPair("filename", "stderr").build();

	testing::internal::CaptureStderr();

	fileWriter->write("Hello, world!");

	// ensure the worker thread has had a chance to write to the stream
	fileWriter.reset();

	std::string output = testing::internal::GetCapturedStderr();

	EXPECT_STREQ(output.c_str(), "Hello, world!\n");
}

TEST(FileLogWriter, writeToStdErrAndStdOut)
{
	auto errWriterBuilder = Herald::createFileWriterBuilder();
	auto errWriter        = errWriterBuilder->addConfigPair("filename", "stderr").build();
	auto outWriterBuilder = Herald::createFileWriterBuilder();
	auto outWriter        = outWriterBuilder->addConfigPair("filename", "stdout").build();

	testing::internal::CaptureStderr();
	testing::internal::CaptureStdout();

	errWriter->write("Hello, stderr!");
	outWriter->write("Hello, stdout!");

	// ensure the worker thread has had a chance to write to the stream
	errWriter.reset();
	outWriter.reset();

	std::string capturedErr = testing::internal::GetCapturedStderr();
	std::string output      = testing::internal::GetCapturedStdout();

	EXPECT_STREQ(capturedErr.c_str(), "Hello, stderr!\n");
	EXPECT_STREQ(output.c_str(), "Hello, stdout!\n");
}

std::filesystem::path tempFile()
{
	std::filesystem::path tempPath = std::filesystem::temp_directory_path() / "herald_test_file.log";
	if (std::filesystem::exists(tempPath)) {
		std::filesystem::remove(tempPath);
	}
	return tempPath;
} // LCOV_EXCL_LINE (false negative when using lcov)

TEST(FileLogWriter, writeToFile)
{
	auto fileWriterBuilder = Herald::createFileWriterBuilder();
	auto tempFilePath      = tempFile();
	auto tempFileName      = tempFilePath.string();
	auto fileWriter        = fileWriterBuilder->addConfigPair("filename", tempFileName).build();

	fileWriter->write("Hello, world!");

	// ensure the worker thread has had a chance to write to the stream
	fileWriter.reset();

	{
		std::ifstream file(tempFilePath);
		std::string   line;
		std::getline(file, line);
		EXPECT_STREQ(line.c_str(), "Hello, world!");
	}
	// remove the temporary file
	tempFile(); // coverage
	std::filesystem::remove(tempFilePath);
}

TEST(FileLogWriter, withJsonLogFormatter)
{
	testing::internal::CaptureStdout();
	{
		auto fileWriterBuilder = Herald::createFileWriterBuilder();
		auto fileWriter        = fileWriterBuilder->addConfigPair("filename", "stdout").build();

		auto jsonTransformerBuilder = Herald::createJsonLogTransformerBuilder();
		auto jsonTransformer        = jsonTransformerBuilder->attachLogWriter(fileWriter).build();

		Herald::log(*jsonTransformer, Herald::LogLevels::Analysis, "Analysis message", "key", "value");
	}

	std::string output = testing::internal::GetCapturedStdout();

	// the order of the key value pairs is not guaranteed so the output needs to contain all the pairs in any order in callbackString
	EXPECT_TRUE(output.find("{") != std::string::npos);
	EXPECT_TRUE(output.find("}") != std::string::npos);
	EXPECT_TRUE(output.find("\"level\": \"Analysis\"") != std::string::npos);
	EXPECT_TRUE(output.find("\"message\": \"Analysis message\"") != std::string::npos);
	EXPECT_TRUE(output.find("\"key\": \"value\"") != std::string::npos);
}

TEST(FileLogWriter, failToOpenFileError)
{
	auto fileWriterBuilder = Herald::createFileWriterBuilder();

	// it will throw std::runtime_error if the file does not exist
	EXPECT_THROW(auto fileWriter = fileWriterBuilder->addConfigPair("filename", "/this/file/does/not/exist").build(), std::runtime_error);
}