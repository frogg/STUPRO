#include "../include/DebugLogger.hpp"

#include <thread>

using namespace std;

void DebugLogger::debug(QString message) {
	cout << std::this_thread::get_id() << ": " << message.toStdString().c_str() << endl;
}
