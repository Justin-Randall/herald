#include "Herald/BaseLogTransformerBuilder.hpp"

namespace Herald
{
	ILogTransformerBuilder & BaseLogTransformerBuilder::addHeader(const std::string & key, const std::string & value)
	{
		headers.push_back({key, value});
		return *this;
	}

	ILogTransformerBuilder & BaseLogTransformerBuilder::attachLogWriter(const ILogWriterPtr writer)
	{
		writers.push_back(writer);
		return *this;
	}

	ILogTransformerBuilder & BaseLogTransformerBuilder::attachLogWriterCallback(const std::function<void(const std::string &)> & callback)
	{
		callbacks.push_back(callback);
		return *this;
	}

	void BaseLogTransformerBuilder::applyBuildConfigurations(ILogTransformerPtr logTransformer)
	{
		for (const auto & callback : callbacks) {
			logTransformer->attachLogWriterCallback(callback);
		}
		for (const auto & [key, value] : headers) {
			logTransformer->addHeader(key, value);
		}
		for (const auto & writer : writers) {
			logTransformer->attachLogWriter(writer.lock());
		}
	}
} // namespace Herald