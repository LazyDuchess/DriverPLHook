#pragma once
#include <map>
#include "Driver.h"

namespace Driver {
	typedef std::map<DWORD, cVehicle*> t_vehicleMap;
	extern t_vehicleMap cVehicleMap;

	void InitializeHooks();
}