#include "Texture.h"
#include <cstdio>

Texture::Texture(): m_textureID(0)
{
	loaded = false;
}

Texture::Texture(const Texture& other): loaded(false), m_textureID(0)
{
}

Texture::~Texture()
{
}

bool Texture::initialize(OpenGL* OpenGL, char* filename, unsigned int textureUnit, bool wrap)
{
	// Load the targa file.
	const bool result = loadTarga(OpenGL, filename, textureUnit, wrap);
	if(!result)
	{
		return false;
	}

	return true;
}

void Texture::shutdown()
{
	// If the texture was loaded then make sure to release it on shutdown.
	if(loaded)
	{
		glDeleteTextures(1, &m_textureID);
		loaded = false;
	}

}

bool Texture::loadTarga(OpenGL* OpenGL, char* filename, unsigned int textureUnit, bool wrap)
{
	FILE* filePtr;
	TargaHeader targaFileHeader;


	// Open the targa file for reading in binary.
	int error = fopen_s(&filePtr, filename, "rb");
	if(error != 0 || !filePtr)
	{
		return false;
	}

	// Read in the file header.
	size_t count = fread(&targaFileHeader, sizeof(TargaHeader), 1, filePtr);
	if(count != 1)
	{
		return false;
	}

	// Get the important information from the header.
	int width = (int)targaFileHeader.width;
	int height = (int)targaFileHeader.height;
	int bpp = (int)targaFileHeader.bpp;

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	const size_t imageSize = width * height * 4;

	// Allocate memory for the targa image data.
	unsigned char* targaImage = new unsigned char[imageSize];
	if(!targaImage)
	{
		return false;
	}

	// Read in the targa image data.
	count = fread(targaImage, 1, imageSize, filePtr);
	if(count != imageSize)
	{
		return false;
	}
	
	// Close the file.
	error = fclose(filePtr);
	if(error != 0)
	{
		return false;
	}

	// Set the unique texture unit in which to store the data.
	OpenGL->glActiveTexture(GL_TEXTURE0 + textureUnit);

	// Generate an ID for the texture.
	glGenTextures(1, &m_textureID);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	// Load the image data into the texture unit.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, targaImage);

	// Set the texture color to either wrap around or clamp to the edge.
	if(wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	// Set the texture filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	// Generate mipmaps for the texture.
	OpenGL->glGenerateMipmap(GL_TEXTURE_2D);

	// Release the targa image data.
	delete [] targaImage;
	targaImage = nullptr;

	// Set that the texture is loaded.
	loaded = true;

	return true;
}
