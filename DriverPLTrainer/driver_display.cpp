#include "pch.h"
#include "Driver.h"

namespace Driver {
	Resolution::Resolution(int width, int height)
	{
		this->width = width;
		this->height = height;
	}

	Resolution cDisplay::GetResolution()
	{
		int resX = ((int*)(modBase + 0x31D4BC))[0];
		int resY = ((int*)(modBase + 0x31D4C0))[0];
		return Resolution(resX, resY);
	}
}