#ifndef _INCLUDED_HeraldJSON_hpp
#define _INCLUDED_HeraldJSON_hpp

#include "Herald/Herald.hpp"
#include "rapidjsoncpp/to_json.hpp"

namespace Herald
{
	template <typename ValueType, typename... Args>
	void log_json(const LogTypes & logType, const std::string & msg,
	              const std::string & key, const ValueType & value,
	              const Args &... args)
	{ // If you see a compiler error here about no matching overload, be sure
	  // the correct to_json method is in scope, e.g. include to_json_vector if
	  // it is a vector of some type. If it is a custom time, ensure you to_json
	  // overload is in scope
		if (!(getEnabledLogTypes() & static_cast<uint32_t>(logType)))
			return;

		// todo : wrap in log envelope with headers and msg
		const std::string timeStamp = getTimeStamp();
		const auto        f         = logTypeNames.find(logType);
		std::string       logTypeName;
		if (f != logTypeNames.end()) {
			logTypeName = (*f).second;
		}

		std::string json =
		    rapidjson::to_json(key, value, "time", timeStamp, "headers",
		                       getJsonLogHeaders(), "log", msg, args...);
		LogInternal::log(logType, json);
	}
} // namespace Herald

#endif //_INCLUDED_HeraldJSON_hpp