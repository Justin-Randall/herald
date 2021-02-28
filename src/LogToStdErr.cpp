#include "Herald/LogToStdErr.hpp"
#include <iostream>

namespace Logger
{
	void logToStdErr(const std::string & log) { std::cerr << log; }
} // namespace Logger
