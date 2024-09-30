// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/ILogWriterBuilder.hpp"
#include "Herald/Logger.hpp"
#include "Herald/SimpleLogTransformerFactory.hpp"
#include "gtest/gtest.h"

class StringLogWriter : public Herald::ILogWriter
{
  public:
	StringLogWriter() = default;
	virtual ILogWriter & addConfigPair(const std::string &, const std::string &) override { return *this; }
	virtual void         write(const std::string & msg) override { logString = msg; }
	std::string          logString;
};

class StringLogWriterBuilder : public Herald::ILogWriterBuilder
{
  public:
	StringLogWriterBuilder() = default;
	virtual Herald::ILogWriterBuilder & addConfigPair(const std::string & key, const std::string & value) override
	{
		configPairs.push_back({key, value});
		return *this;
	}

	virtual Herald::ILogWriterPtr build() override
	{
		auto logWriter = std::make_shared<StringLogWriter>();
		for (const auto & [key, value] : configPairs) {
			logWriter->addConfigPair(key, value);
		}
		return logWriter;
	}

  private:
	std::vector<std::pair<std::string, std::string>> configPairs;
};

Herald::ILogWriterBuilderPtr createStringLogWriterBuilder() { return std::make_unique<StringLogWriterBuilder>(); }

TEST(ILogTransformer, logMetaDataStringCallback)
{
	auto logWriterBuilder      = createStringLogWriterBuilder();
	auto logWriter             = logWriterBuilder->addConfigPair("key", "value").addConfigPair("one", "two").build();
	auto logTransformerBuilder = Herald::createSimpleLogTransformerBuilder();

	// auto logWriter             = std::make_shared<StringLogWriter>();
	auto logTransformer = logTransformerBuilder->attachLogWriter(logWriter).build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message", "key", "value");

	StringLogWriter * s = dynamic_cast<StringLogWriter *>(logWriter.get());
	EXPECT_STREQ(s->logString.c_str(), "[Analysis] Analysis message [key=value]");
}
