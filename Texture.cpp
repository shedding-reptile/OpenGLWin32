#include "Texture.h"
#include <cstdio>
#include <exception>
#include "glad/glad.h"

Texture::Texture(const char* filename, unsigned int textureUnit, bool wrap): loaded(false), textureID(0)
{
	// Load the targa file.
	if (!loadTarga(filename, textureUnit, wrap))
	{
		throw std::exception("Cannot initialize texture!");
	}
}

Texture::~Texture()
{
	// If the texture was loaded then make sure to release it on shutdown.
	if (loaded)
	{
		glDeleteTextures(1, &textureID);
		loaded = false;
	}
}

bool Texture::loadTarga(const char* filename, unsigned int textureUnit, bool wrap)
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
    const int width = static_cast<int>(targaFileHeader.width);
    const int height = static_cast<int>(targaFileHeader.height);
    const int bpp = static_cast<int>(targaFileHeader.bpp);

	// Check that it is 32 bit and not 24 bit.
	if(bpp != 32)
	{
		return false;
	}

	// Calculate the size of the 32 bit image data.
	const int imageSize = width * height * 4;

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
	glActiveTexture(GL_TEXTURE0 + textureUnit);

	// Generate an ID for the texture.
	glGenTextures(1, &textureID);

	// Bind the texture as a 2D texture.
	glBindTexture(GL_TEXTURE_2D, textureID);

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
	glGenerateMipmap(GL_TEXTURE_2D);

	// Release the targa image data.
	delete [] targaImage;
	targaImage = nullptr;

	// Set that the texture is loaded.
	loaded = true;

	return true;
}
