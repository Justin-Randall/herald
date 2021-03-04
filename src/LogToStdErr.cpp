#include "Herald/LogToStdErr.hpp"
#include <iostream>

namespace Herald
{
	void logToStdErr(const std::string & log) { std::cerr << log; }
} // namespace Herald
