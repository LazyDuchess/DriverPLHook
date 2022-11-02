#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"
#include <iostream>
#include <time.h>
#include <string>
#include <vector>

#define CHARACTER_OFFSET 0x63C
#define MODEL_OFFSET 0x744
#define MODEL_OFFSET2 0x764
#define HEALTH_OFFSET 0x684
#define INCAR_OFFSET 0xFC
#define CAR_OFFSET 0x688

#define PED_DAMAGE_FUNC 0xEC5DD

namespace Driver {

	cPed* PlayerPed;

	cPed::cPed(DWORD addr)
	{
		address = addr;
	}

	cPed::cPed()
	{
		address = NULL;
	}

	char* damageFuncAddr;

	__declspec(naked) void damageHook(DWORD address, float damage, char unknown)
	{
		__asm {
			//char
			push [esp+0x0C]
			//damage
			push [esp+0x0C]
			//native ped class pointer
			mov ecx,[esp+0x0C]
			//native damage function
			call damageFuncAddr
			ret
		}
	}

	/// <summary>
	/// Damage is in HP - 1.0 is full hp.
	/// </summary>
	/// <param name="damage"></param>
	void cPed::Damage(float damage, bool unk)
	{
		damageFuncAddr = modBase + PED_DAMAGE_FUNC;
		damageHook(address, damage, unk ? 1 : 0);
	}

	//TODO - adding vehicles to the map here is not ideal, but's been put as a workaround for a crash in the Steal to Order sidemission
	//related to the tow truck not calling the hooked constructor that puts it in the vehiclemap.
	cVehicle* cPed::GetVehicle()
	{
		DWORD vehicleAddress = ((DWORD*)(address + CAR_OFFSET))[0];
		if (vehicleAddress == NULL)
			return NULL;
		cVehicle* finalVehicle = cVehicleMap[vehicleAddress];
		if (finalVehicle != NULL)
			return finalVehicle;
		finalVehicle = new cVehicle(vehicleAddress);
		cVehicleMap[vehicleAddress] = finalVehicle;
		return finalVehicle;
	}

	DWORD cPed::GetVehiclePointer()
	{
		DWORD vehicleAddress = ((DWORD*)(address + CAR_OFFSET))[0];
		return vehicleAddress;
	}

	bool cPed::InVehicle() 
	{
		DWORD vehicleAddress = ((DWORD*)(address + CAR_OFFSET))[0];
		if (vehicleAddress == NULL)
			return false;
		return true;
	}

	float cPed::GetHealth()
	{
		return ((float*)(address + HEALTH_OFFSET))[0];
	}
	void cPed::SetHealth(float health)
	{
		if (address == NULL)
			return;
		((float*)(address + HEALTH_OFFSET))[0] = health;
	}
	int cPed::GetCharacter()
	{
		return ((int*)(address + CHARACTER_OFFSET))[0];
	}
	int cPed::GetModel()
	{
		return ((int*)(address + MODEL_OFFSET))[0];
	}
	void cPed::SetCharacter(int character)
	{
		if (address == NULL)
			return;
		((int*)(address + CHARACTER_OFFSET))[0] = character;
	}
	void cPed::SetModel(int model)
	{
		if (address == NULL)
			return;
		((int*)(address + MODEL_OFFSET))[0] = model;
		((int*)(address + MODEL_OFFSET2))[0] = model;
	}

	bool cPed::operator==(const cPed& other) {
		if (address == other.address)
			return true;
		return false;
	}

	bool cPed::operator!=(const cPed& other) {
		return !(*this == other);
	}

	cPed* cPed::GetPlayer()
	{
		DWORD addr = (DWORD)modBase + 0x0030C6D8;
		if (Hooking::memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			addr += 0x1C;
			if (Hooking::memory_readable((DWORD*)addr, 4))
			{
				memcpy_s(&addr, 4, (DWORD*)addr, 4);
				return cPedMap[addr];
			}
		}
		return NULL;
	}

	t_pedVector cPed::GetPeds()
	{
		t_pedVector pedVector = t_pedVector();
		int i = 0;
		for (const auto& elem : cPedMap)
		{
			pedVector.push_back(elem.second);
			i++;
		}
		return pedVector;
	}

	Character* characters[]{
		new Character(173, 49),
		new Character(176, 62),
		new Character(56, 29),
		new Character(112, 65),
		new Character(142, 99),
		new Character(24, 40),
		new Character(201, 20),
		new Character(84, 98),
		new Character(1, 96),
		new Character(96, 104),
		new Character(113, 56),
		new Character(143, 100)
	};

	Character::Character(int model, int character)
	{
		this->model = model;
		this->character = character;
	}

	Character* Character::GetRandom()
	{
		int RanIndex = rand() % sizeof(characters)/sizeof(characters[0]);
		return characters[RanIndex];
	}
}