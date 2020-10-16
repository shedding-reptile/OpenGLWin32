#pragma once

class Input
{
public:
	Input();
	~Input();
	void keyDown(unsigned int);
	void keyUp(unsigned int);
	bool isKeyDown(unsigned int);

private:
	bool keys[256];
};
