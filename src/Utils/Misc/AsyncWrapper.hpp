#ifndef SRC_UTILS_MISC_ASYNCWRAPPER_HPP_
#define SRC_UTILS_MISC_ASYNCWRAPPER_HPP_

#include <condition_variable>
#include <mutex>

/**
 * Class that waits until an asynchronous operation has completed.
 *
 * The asynchronous operation's callback should execute "wake()" on this object.
 */
class AsyncWrapper
{
public:

	AsyncWrapper();

	void sleep();
	void wake();

private:

	std::mutex mutex;
	std::condition_variable condition;
	bool awake;
};

#endif
