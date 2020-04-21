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
	Texture();
	Texture(const Texture&);
	~Texture();
	bool initialize(OpenGL*, char*, unsigned int, bool);
	void shutdown();

private:
	bool loadTarga(OpenGL*, char*, unsigned int, bool);

	bool loaded;
	unsigned int m_textureID;
};
