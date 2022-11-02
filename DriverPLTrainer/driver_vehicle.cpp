#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"
#include <iostream>
#include <time.h>
#include <string>
#include <vector>

#define VEHICLE_DAMAGE_OFFSET 0x574
#define VEHICLE_EXPLOSION_OFFSET 0x4D0

#define VEHICLE_R_OFFSET 0x8C
#define VEHICLE_G_OFFSET 0x90
#define VEHICLE_B_OFFSET 0x94

//VTABLE
#define VEHICLE_VTABLE_REPAIR 0x38

namespace Driver {

	void cVehicle::SetColor(Color color)
	{
		((float*)(address + VEHICLE_R_OFFSET))[0] = color.red;
		((float*)(address + VEHICLE_G_OFFSET))[0] = color.green;
		((float*)(address + VEHICLE_B_OFFSET))[0] = color.blue;
	}

	Color cVehicle::GetColor()
	{
		float r = ((float*)(address + VEHICLE_R_OFFSET))[0];
		float g = ((float*)(address + VEHICLE_G_OFFSET))[0];
		float b = ((float*)(address + VEHICLE_B_OFFSET))[0];
		return Color(r, g, b);
	}

	cVehicle::cVehicle(DWORD addr)
	{
		address = addr;
	}

	float cVehicle::GetDamage()
	{
		return ((float*)(address + VEHICLE_DAMAGE_OFFSET))[0];
	}

	void cVehicle::SetDamage(float damage)
	{
		((float*)(address + VEHICLE_DAMAGE_OFFSET))[0] = damage;
	}

	void cVehicle::Explode()
	{
		((float*)(address + VEHICLE_EXPLOSION_OFFSET))[0] = 0.1;
	}

	__declspec(naked) void callVehicleRepair(DWORD address)
	{
		__asm {
			//pull address param
			mov ecx, [esp + 0x4]
			//move vtable address into eax
			mov eax, [ecx]
			//call repair func from vtable
			call[eax + 0x38]
			ret
		}
	}

	bool cVehicle::operator==(const cVehicle& other) {
		if (address == other.address)
			return true;
		return false;
	}

	bool cVehicle::operator!=(const cVehicle& other) {
		return !(*this == other);
	}

	void cVehicle::Repair()
	{
		callVehicleRepair(address);
	}

	t_vehicleVector cVehicle::GetVehicles()
	{
		t_vehicleVector vehicleVector = t_vehicleVector();
		int i = 0;
		for (const auto& elem : cVehicleMap)
		{
			vehicleVector.push_back(elem.second);
			i++;
		}
		return vehicleVector;
	}
}