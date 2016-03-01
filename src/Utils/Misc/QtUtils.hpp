#ifndef KRONOS_UTILS_MISC_QT_UTILS_HPP
#define KRONOS_UTILS_MISC_QT_UTILS_HPP

#include <QCoreApplication>
#include <QEvent>

template <typename F>
static void postToMainThread(F&& function) {
	struct Event : public QEvent {
		F function;
		Event(F&& function) : QEvent(QEvent::None), function(std::move(function)) { }
		~Event() {
			function();
		}
	};
	QCoreApplication::postEvent(QCoreApplication::instance(), new Event(std::move(function)));
}

#endif // KRONOS_UTILS_MISC_QT_UTILS_HPP
