/*
 * Globe.cpp
 *
 *  Created on: Oct 23, 2015
 *      Author: bbq
 */

#include "Globe.hpp"

Globe::Globe(vtkRenderWindow & renderWindow) :
		myRenderWindow(renderWindow)
{
}

Globe::~Globe()
{
    // TODO Auto-generated destructor stub
}

vtkSmartPointer<vtkPolyDataMapper> Globe::getPlaneMapper() const
{

}

vtkRenderWindow &Globe::getRenderWindow() const
{
    return myRenderWindow;
}

