// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/BaseLogTransformer.hpp"
#include "Herald/BaseLogTransformerBuilder.hpp"
#include "Herald/ILogWriter.hpp"
#include "Herald/Logger.hpp"
#include "Herald/SimpleLogTransformerFactory.hpp"
#include <algorithm>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

namespace Herald
{
	class LogTransformer : public BaseLogTransformer
	{
	  public:
		LogTransformer()           = default;
		~LogTransformer() override = default;

		virtual void log(const LogEntry & entry) override
		{
			if (!isLogLevelEnabled(entry.logLevel))
				return;

			std::stringstream ss;

			// inject the headers
			for (const auto & [key, value] : headers) {
				ss << "[" << key << "=" << value << "] ";
			}

			ss << "[" << logTypeNames.at(entry.logLevel) << "] " << entry.message;
			for (const auto & [key, value] : entry.metadata) {
				ss << " [" << key << "=" << value << "]";
			}
			for (const auto & callback : callbacks) {
				callback(ss.str());
			}

			for (const auto & writer : writers) {
				if (auto w = writer.lock()) {
					w->write(ss.str());
				}
			}
		}
	};

	class LogTransformerBuilder : public BaseLogTransformerBuilder
	{
	  public:
		LogTransformerBuilder()           = default;
		~LogTransformerBuilder() override = default;
		ILogTransformerPtr build() override
		{
			auto logger = std::make_shared<LogTransformer>();
			applyBuildConfigurations(logger);
			return logger;
		}
	};

	// Factory function that returns an opaque ILogTransformerBuilderBase pointer
	ILogTransformerBuilderPtr createSimpleLogTransformerBuilder() { return std::make_unique<LogTransformerBuilder>(); }
} // namespace Herald
