#include "Herald/BaseLogTransformer.hpp"

namespace Herald
{
	ILogTransformer & BaseLogTransformer::addHeader(const std::string & key, const std::string & value)
	{
		headers[key] = value;
		return *this;
	}

	ILogTransformer & BaseLogTransformer::attachLogWriterCallback(const std::function<void(const std::string &)> & callback)
	{
		callbacks.push_back(callback);
		return *this;
	}

	ILogTransformer & BaseLogTransformer::attachLogWriter(const ILogWriterPtr writer)
	{
		writers.push_back(writer);
		return *this;
	}

	void BaseLogTransformer::removeLogCallback(const std::function<void(const std::string &)> & callback)
	{
		callbacks.erase(std::remove_if(callbacks.begin(), callbacks.end(),
		                               [&](const auto & cb) {
			                               return cb.target_type() == callback.target_type() &&
			                                      cb.template target<void(const std::string &)>() ==
			                                          callback.template target<void(const std::string &)>();
		                               }),
		                callbacks.end());
	}

	void BaseLogTransformer::removeHeader(const std::string & key) { headers.erase(key); }
} // namespace Herald
