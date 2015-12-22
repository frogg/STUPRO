#ifndef NDEBUG

#include <Utils/Misc/DebugLog.hpp>

const std::string DebugLog::DEBUG_LOGFILE = "debug.log";

DebugLog * DebugLog::instance = nullptr;

DebugLog & DebugLog::get()
{
	if (instance == nullptr)
	{
		instance = new DebugLog;
	}
	
	return *instance;
}

DebugLog::DebugLog() : std::ofstream(DEBUG_LOGFILE.c_str(), std::ios::trunc)
{
}

#endif
