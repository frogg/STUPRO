/*
 * SlotCallback.cpp
 *
 *  Created on: Feb 12, 2016
 *      Author: bbq
 */

#include <Utils/Misc/SlotCallback.hpp>

SlotCallback::SlotCallback(std::function<void(void*)> callback) :
	callback(callback) {
}

SlotCallback::~SlotCallback() {
}

void SlotCallback::callbackSlot() {
	callback(nullptr);
}

void SlotCallback::callbackSlotWithUserData(void* userData) {
	callback(userData);
}
