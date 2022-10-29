#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"
#include <iostream>
#include <time.h>
#include <string>

#define CHARACTER_OFFSET 0x63C
#define MODEL_OFFSET 0x744
#define MODEL_OFFSET2 0x764
#define HEALTH_OFFSET 0x684
#define INCAR_OFFSET 0xFC
#define CAR_OFFSET 0x688

#define PLAYER_POINTER 0x30C7A0
#define PLAYER_MONEY_OFFSET 0x884

#define WANTED_POINTER 0x0030C5C8
//#define WANTED_WANTED_OFFSET 0x24
//#define WANTED_SUS_OFFSET 0x28
//#define WANTED_ENGAGING_OFFSET 0x39
//#define WANTED_HIDDEN_OFFSET 0x3C

#define WANTED_WANTED_OFFSET 0x24
#define WANTED_WANTED_OFFSET2 0x20
#define WANTED_SUS_OFFSET 0x28
#define WANTED_ENGAGING_OFFSET 0x39
#define WANTED_HIDDEN_OFFSET 0x3C
#define WANTED_SUSPECTING_OFFSET 0x3B

#define VEHICLE_DAMAGE_OFFSET 0x574

namespace Driver {

	char* modBase;

	void SetModuleBase(char* moduleBase) {
		modBase = moduleBase;
		srand(time(NULL));
		InitializeHooks();
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

	cWanted* PlayerWanted;

	cWanted::cWanted(DWORD addr)
	{
		address = addr;
	}

	cWanted* cWanted::Get()
	{
		DWORD addr = (DWORD)modBase + WANTED_POINTER;
		if (Hooking::memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			if (PlayerWanted != NULL)
			{
				if (PlayerWanted->address == addr)
					return PlayerWanted;
				else
					delete PlayerWanted;
			}
			PlayerWanted = new cWanted(addr);
			return PlayerWanted;
		}
		return NULL;
	}

	float cWanted::GetSuspicionLevel()
	{
		return ((float*)(address + WANTED_SUS_OFFSET))[0];
	}

	float cWanted::GetWantedLevel()
	{
		return ((float*)(address + WANTED_WANTED_OFFSET))[0];
	}

	bool cWanted::GetEngaging()
	{
		return ((char*)(address + WANTED_ENGAGING_OFFSET))[0] == 1;
	}

	bool cWanted::GetHidden()
	{
		return ((char*)(address + WANTED_HIDDEN_OFFSET))[0] == 1;
	}

	bool cWanted::GetSuspecting()
	{
		return ((char*)(address + WANTED_SUSPECTING_OFFSET))[0] == 1;
	}

	void cWanted::ClearWantedLevel() {
		SetSuspicionLevel(0.0);
		SetWantedLevel(0.0);
		SetHidden(false);
		SetEngaging(false);
		SetSuspecting(false);
	}

	void cWanted::SetSuspicionLevel(float level)
	{
		((float*)(address + WANTED_SUS_OFFSET))[0] = level;
	}

	void cWanted::SetWantedLevel(float level)
	{
		((float*)(address + WANTED_WANTED_OFFSET))[0] = level;
		((float*)(address + WANTED_WANTED_OFFSET2))[0] = level;
	}

	void cWanted::SetEngaging(bool engaging)
	{
		((char*)(address + WANTED_ENGAGING_OFFSET))[0] = engaging ? 1 : 0;
	}

	void cWanted::SetHidden(bool hidden)
	{
		((char*)(address + WANTED_HIDDEN_OFFSET))[0] = hidden ? 1 : 0;
	}

	void cWanted::SetSuspecting(bool suspecting)
	{
		((char*)(address + WANTED_SUSPECTING_OFFSET))[0] = suspecting ? 1 : 0;
	}

	cPlayer* PlayerSingleton;

	cPlayer::cPlayer(DWORD addr)
	{
		address = addr;
	}

	void cPlayer::SetMoney(int money)
	{
		((int*)(address + PLAYER_MONEY_OFFSET))[0] = money;
	}

	int cPlayer::GetMoney()
	{
		return ((int*)(address + PLAYER_MONEY_OFFSET))[0];
	}

	cPlayer* cPlayer::Get()
	{
		DWORD addr = (DWORD)modBase + PLAYER_POINTER;
		if (Hooking::memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			if (PlayerSingleton != NULL)
			{
				if (PlayerSingleton->address == addr)
					return PlayerSingleton;
				else
					delete PlayerSingleton;
			}
			PlayerSingleton = new cPlayer(addr);
			return PlayerSingleton;
		}
		return NULL;
	}

	cPed* PlayerPed;

	cPed::cPed(DWORD addr)
	{
		address = addr;
	}

	cPed::cPed()
	{
		address = NULL;
	}

	cVehicle* cPed::GetVehicle()
	{
		DWORD vehicleAddress = ((DWORD*)(address + CAR_OFFSET))[0];
		if (vehicleAddress == NULL)
			return NULL;
		return cVehicleMap[vehicleAddress];
	}

	bool cPed::InVehicle() 
	{
		DWORD vehicleAddress = ((DWORD*)(address + CAR_OFFSET))[0];
		if (vehicleAddress == NULL)
			return false;
		return true;
		//return ((char*)(address + INCAR_OFFSET))[0] == 1;
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
				if (PlayerPed != NULL)
				{
					if (PlayerPed->address == addr)
						return PlayerPed;
					else
						delete PlayerPed;
				}
				PlayerPed = new cPed(addr);
				return PlayerPed;
			}
		}
		return NULL;
	}

	cPed * cPed::GetPeds()
	{
		DWORD addr = (DWORD)modBase + 0x0030C6E0;
		if (Hooking::memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			addr += 0x31C;
			if (Hooking::memory_readable((DWORD*)addr, 4))
			{
				DWORD* pointers = (DWORD*)addr;
				cPed pedArray[PED_AMOUNT];
				for (int i = 0; i < PED_AMOUNT; i++)
				{
					/*
					wprintf(std::to_wstring(pointers[i]).c_str());
					wprintf(L"\n");*/
					if (pointers[i] != NULL)
					{
						pedArray[i] = cPed(pointers[i]);
					}
					else
					{
						pedArray[i] = NULL;
					}
				}
				return pedArray;
			}
		}
		return NULL;
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