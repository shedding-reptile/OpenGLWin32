#include "Input.h"
#include <cstring>

Input::Input(const Input& other): m_keys{}
{
}

Input::~Input()
= default;

void Input::initialize()
{
	memset(m_keys, 0, sizeof(m_keys));

}

void Input::keyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
}

void Input::keyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
}

bool Input::isKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return m_keys[key];
}
