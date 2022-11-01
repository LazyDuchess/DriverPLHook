#include "pch.h"
#include "Input.h"
#include <Windows.h>

namespace Input {
	bool KeyPressed(int vKey)
	{
		return ((GetAsyncKeyState(vKey) & 0x8001) == 0x8001);
	}
	bool KeyDown(int vKey)
	{
		return GetAsyncKeyState(vKey) & 0x8000;
	}
}