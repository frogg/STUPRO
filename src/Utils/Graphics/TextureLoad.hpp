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

class QImage;

/**
 * Creates a VTK texture with an alpha channel from an RGB image and an image whose red channel will serve as the alpha
 * channel for the created texture.
 * 
 * @param rgb
 *        Contains the RGB image data to be loaded into the texture.
 * @param alpha
 *        Contains the Alpha channel data (read from the Red component) to be loaded into the texture.
 * 
 * @return A smart pointer to a texture holding the combined image.
 */
vtkSmartPointer<vtkOpenGLTexture> loadAlphaTexture(const QImage& rgb, const QImage& alpha);

#endif /* TEXTURELOAD_HPP_ */
