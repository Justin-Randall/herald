#include "Herald/HeraldJSON.hpp"
#include "rapidjsoncpp/to_json_map.hpp"
#include "rapidjsoncpp/to_json_set.hpp"
#include "rapidjsoncpp/to_json_unordered_map.hpp"
#include "rapidjsoncpp/to_json_vector.hpp"
#include "gtest/gtest.h"
#include <atomic>
#include <condition_variable>
#include <mutex>

namespace
{
	std::string             lastLogCallbackMessage;
	std::mutex              conditionLock;
	std::condition_variable condition;

	void logCallback(const std::string & message)
	{
		condition.notify_one();
		lastLogCallbackMessage = message;
		std::cout << message;
	}

	void waitForLogMessage()
	{
		std::unique_lock<std::mutex> l(conditionLock);
		condition.wait(l);
	}
} // namespace

TEST(HeraldJSON, VerifySeralizationTypes)
{
	Herald::install();
	Herald::setJsonLogHeader("userId", "test user");
	Herald::setJsonLogHeader("hostName", "test machine");

	Herald::addLogMessageCallback(&logCallback);

	Herald::enableLogType(Herald::LogTypes::Debug);

	const unsigned char              logUnsignedCharValue  = 200;
	const char                       logCharValue          = -100;
	const unsigned short             logUnsignedShortValue = 65530;
	const short                      logShortValue         = -30000;
	const unsigned int               logUnsignedIntValue   = -1000000000;
	const int                        logIntValue           = 1000000042;
	const uint64_t                   logUnsignedInt64      = 1000000000000;
	const int64_t                    logInt64              = -1000000000000;
	const bool                       logBoolValue          = true;
	const float                      logFloatValue         = 3.1415f;
	const double                     logDoubleValue        = 3.1415926535897;
	const std::string                logStringValue   = "Hello JSON logging!";
	const std::vector<int>           logIntArray      = {0, 1, 2, 3};
	const std::set<int>              logIntSet        = {4, 5, 6, 7};
	const std::map<std::string, int> logMapStringInts = {
	    {"answer to life, the universe and everything", 42},
	    {"one", 1},
	    {"two", 2}};
	const std::unordered_map<std::string, float> logUnorderedMapStringFloats = {
	    {"answer to life, the universe and everything", 42.1f},
	    {"one", 1.2f},
	    {"two", 2.3f}};

	Herald::log_json(Herald::LogTypes::Debug, "test unsigned char",
	                 "logUnsignedCharValue", logUnsignedCharValue);
	waitForLogMessage();

	EXPECT_TRUE(lastLogCallbackMessage.find("time") != std::string::npos);

	EXPECT_TRUE(lastLogCallbackMessage.find("headers") != std::string::npos);

	EXPECT_TRUE(lastLogCallbackMessage.find("\"hostName\": \"test machine\"") !=
	            std::string::npos);

	EXPECT_TRUE(lastLogCallbackMessage.find("\"userId\": \"test user\"") !=
	            std::string::npos);

	EXPECT_TRUE(lastLogCallbackMessage.find("test unsigned char") !=
	            std::string::npos);
	EXPECT_TRUE(lastLogCallbackMessage.find("\"logUnsignedCharValue\": 200") !=
	            std::string::npos);
	EXPECT_TRUE(lastLogCallbackMessage.find("\"userId\": \"test user\"") !=
	            std::string::npos);

	Herald::log_json(Herald::LogTypes::Debug, "test char", "logCharValue",
	                 logCharValue);

	Herald::log_json(Herald::LogTypes::Debug, "test unsigned short",
	                 "logUnsignedShortValue", logUnsignedShortValue);

	Herald::log_json(Herald::LogTypes::Debug, "test short", "logShortValue",
	                 logShortValue);

	Herald::log_json(Herald::LogTypes::Debug, "test unsigned int",
	                 "logUnsignedIntValue", logUnsignedIntValue);

	Herald::log_json(Herald::LogTypes::Debug, "test int", "logIntValue",
	                 logIntValue);

	Herald::log_json(Herald::LogTypes::Debug, "test unsigned int64",
	                 "logUnsignedInt64", logUnsignedInt64);

	Herald::log_json(Herald::LogTypes::Debug, "test int64", "logInt64",
	                 logInt64);

	Herald::log_json(Herald::LogTypes::Debug, "test boolean", "logBoolValue",
	                 logBoolValue);

	Herald::log_json(Herald::LogTypes::Debug, "test float", "logFloatValue",
	                 logFloatValue);

	Herald::log_json(Herald::LogTypes::Debug, "test double", "logDoubleValue",
	                 logDoubleValue);

	Herald::log_json(Herald::LogTypes::Debug, "test string", "logStringValue",
	                 logStringValue);

	Herald::log_json(Herald::LogTypes::Debug, "test int array", "logIntArray",
	                 logIntArray);

	Herald::log_json(Herald::LogTypes::Debug, "test int set", "logSetArray",
	                 logIntSet);
	Herald::log_json(Herald::LogTypes::Debug, "test string int map",
	                 "logMapStringInts", logMapStringInts);
	Herald::log_json(
	    Herald::LogTypes::Debug, "test string float unorddered map",
	    "logUnorderedMapStringFloats", logUnorderedMapStringFloats);
}
