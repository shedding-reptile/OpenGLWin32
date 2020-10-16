#pragma once

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
	Texture(const char* filename, unsigned int textureUnit, bool wrap);
	~Texture();

private:
	bool loadTarga(const char*, unsigned int, bool);

	bool loaded;
	unsigned int textureID;
};
