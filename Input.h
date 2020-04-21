#pragma once

class Input
{
public:
	Input() = default;
	Input(const Input&);
	~Input();
	void initialize();
	void keyDown(unsigned int);
	void keyUp(unsigned int);
	bool isKeyDown(unsigned int);

private:
	bool m_keys[256];
};
