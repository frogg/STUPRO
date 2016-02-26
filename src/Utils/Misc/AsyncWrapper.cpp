#include <AsyncWrapper.hpp>

AsyncWrapper::AsyncWrapper() :
	awake(false) {
}

void AsyncWrapper::sleep() {
	std::unique_lock<std::mutex> lock(mutex);
	condition.wait(lock, [this] {
		return awake;
	});
}

void AsyncWrapper::wake() {
	std::unique_lock<std::mutex> lock(mutex);
	awake = true;
	condition.notify_one();
}
