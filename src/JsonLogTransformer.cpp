// Copyright 2016-2024 Playscale Ptd Ltd and Justin Randall
// MIT License, see LICENSE file for full details.

#include "Herald/BaseLogTransformer.hpp"
#include "Herald/BaseLogTransformerBuilder.hpp"
#include "Herald/GetTimeStamp.hpp"
#include "Herald/JsonLogTransformerFactory.hpp"
#include "rapidjsoncpp/to_json.hpp"
#include "rapidjsoncpp/to_json_map.hpp"

namespace rapidjson
{
	// add a writer for LogEntry
	template <typename WriterType> void write(WriterType & w, const Herald::LogEntry & value)
	{
		w.StartObject();
		w.Key("level");
		w.String(Herald::logTypeNames.at(value.logLevel).c_str());
		w.Key("message");
		w.String(value.message.c_str());
		for (const auto & [k, v] : value.metadata) {
			w.Key(k.c_str());
			w.String(v.c_str());
		}
		w.EndObject();
	}
} // namespace rapidjson

namespace Herald
{
	class JsonLogTransformer : public BaseLogTransformer
	{
	  public:
		JsonLogTransformer()           = default;
		~JsonLogTransformer() override = default; // LCOV_EXCL_LINE

		virtual void log(const LogEntry & entry) override
		{
			if (!isLogLevelEnabled(entry.logLevel))
				return;

			// use json to log the message
			const std::string timeStamp = getTimeStamp();
			const auto        f         = logTypeNames.find(entry.logLevel);
			std::string       logTypeName;
			if (f != logTypeNames.end()) {
				logTypeName = (*f).second;
			}

			std::string json = rapidjson::to_json("log", entry, "headers", headers, "timestamp", timeStamp);

			// ship it to the callbacks
			for (const auto & callback : callbacks) {
				callback(json);
			}

			// ship it to the writers
			for (const auto & writer : writers) {
				if (auto w = writer.lock()) {
					if (w)
						w->write(json);
				}
			}
		}
	};

	class JsonLogTransformerBuilder : public BaseLogTransformerBuilder
	{
		virtual ILogTransformerPtr build() override
		{
			auto logger = std::make_shared<JsonLogTransformer>();
			applyBuildConfigurations(logger);
			return logger;
		}
	};

	ILogTransformerBuilderPtr createJsonLogTransformerBuilder() { return std::make_unique<JsonLogTransformerBuilder>(); }
} // namespace Herald