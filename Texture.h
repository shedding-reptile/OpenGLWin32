#pragma once

#include "OpenGL.h"

class Texture
{
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

public:
	Texture(char* filename, unsigned int textureUnit, bool wrap);
	~Texture();

private:
	bool loadTarga(char*, unsigned int, bool);

	bool loaded;
	unsigned int textureID;
};
