/*
 * TextureLoad.hpp
 *
 *  Created on: Oct 30, 2015
 *      Author: bbq
 */

#ifndef TEXTURELOAD_HPP_
#define TEXTURELOAD_HPP_

#include <vtkOpenGLTexture.h>
#include <vtkSmartPointer.h>
#include <string>

vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(std::string rgbFile, std::string alphaFile);

#endif /* TEXTURELOAD_HPP_ */
