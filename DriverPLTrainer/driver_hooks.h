#pragma once
#include <map>
#include "Driver.h"

namespace Driver {
	typedef std::map<DWORD, cVehicle*> t_vehicleMap;
	typedef std::map<DWORD, cPed*> t_pedMap;
	extern t_vehicleMap cVehicleMap;
	extern t_pedMap cPedMap;

	/// <summary>
	/// Initializes all hooks into game code, to enable extra functionality.
	/// </summary>
	void InitializeHooks();
}