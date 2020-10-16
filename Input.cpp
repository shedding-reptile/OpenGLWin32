#include "Input.h"

Input::Input():
	keys{ false }
{
}

Input::~Input()
= default;

void Input::keyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	keys[input] = true;
}

void Input::keyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	keys[input] = false;
}

bool Input::isKeyDown(unsigned int key)
{
	// Return what state the key is in (pressed/not pressed).
	return keys[key];
}
