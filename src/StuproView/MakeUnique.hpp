/*
 * MakeUnique.hpp
 *
 *  Created on: Oct 28, 2015
 *      Author: bbq
 */

#ifndef MAKEUNIQUE_HPP_
#define MAKEUNIQUE_HPP_

#include <memory>

template<typename T, typename ...Args>
std::unique_ptr<T> makeUnique(Args && ...args)
{
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif /* MAKEUNIQUE_HPP_ */
