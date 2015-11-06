/*
 * Main.cpp
 *
 *  Created on: Sep 23, 2015
 *      Author: marukyu
 */

#include <qcoreapplication.h>
#include "StuproApplication.hpp"

int main(int argc, char ** argv)
{
	// initialize and run a QApplication which is needed for some QT functionality
	QCoreApplication app(argc, argv);
	
	StuproApplication client;

	client.run();

	return 0;
}
