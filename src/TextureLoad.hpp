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
#include <QImage>

vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(const QImage& rgb, const QImage& alpha);

#endif /* TEXTURELOAD_HPP_ */
