// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/Logger.hpp"
#include "Herald/SimpleLogTransformerFactory.hpp"
#include "Herald/WriterBuilder.hpp"
// #include "Herald/TransformerBuilder.hpp"
#include "gtest/gtest.h"

class StringLogWriter : public Herald::ILogWriter
{
  public:
	StringLogWriter() = default;
	virtual ILogWriter & addConfigPair(const std::string &, const std::string &) override { return *this; }
	virtual void         write(const std::string & msg) override { logString = msg; }
	std::string          logString;
};

TEST(ILogTransformer, logMetaDataStringCallback)
{
	auto logWriter      = Herald::createWriterBuilder<StringLogWriter>()->addConfigPair("key", "value").addConfigPair("one", "two").build();
	auto logTransformer = Herald::createSimpleLogTransformerBuilder()->attachLogWriter(logWriter).build();

	Herald::log(*logTransformer, Herald::LogLevels::Analysis, "Analysis message", "key", "value");

	StringLogWriter * s = dynamic_cast<StringLogWriter *>(logWriter.get());
	EXPECT_STREQ(s->logString.c_str(), "[Analysis] Analysis message [key=value]");
}
