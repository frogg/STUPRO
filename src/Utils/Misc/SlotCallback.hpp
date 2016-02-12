/*
 * SlotCallback.hpp
 *
 *  Created on: Feb 12, 2016
 *      Author: bbq
 */

#ifndef SRC_UTILS_MISC_SLOTCALLBACK_HPP_
#define SRC_UTILS_MISC_SLOTCALLBACK_HPP_

#include <functional>
#include <qobject.h>

class SlotCallback: public QObject
{
Q_OBJECT

public:
	SlotCallback(std::function<void(void*)> callback);
	virtual ~SlotCallback();
	
public slots:
	
	void callbackSlot();
	void callbackSlotWithUserData(void * userData);
	
private:
	
	std::function<void(void*)> callback;
};

#endif /* SRC_UTILS_MISC_SLOTCALLBACK_HPP_ */
