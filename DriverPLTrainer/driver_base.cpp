#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"
#include <iostream>
#include <time.h>
#include <string>
#include <vector>

#define PLAYER_POINTER 0x30C7A0
#define PLAYER_MONEY_OFFSET 0x884

#define WANTED_POINTER 0x0030C5C8

#define WANTED_WANTED_OFFSET 0x24
#define WANTED_WANTED_OFFSET2 0x20
#define WANTED_SUS_OFFSET 0x28
#define WANTED_ENGAGING_OFFSET 0x39
#define WANTED_HIDDEN_OFFSET 0x3C
#define WANTED_SUSPECTING_OFFSET 0x3B
#define WANTED_ALERT_OFFSET 0x1C

namespace Driver {

	char* modBase;

	void SetModuleBase(char* moduleBase) {
		modBase = moduleBase;
		srand(time(NULL));
		InitializeHooks();
	}

	Color::Color(float r, float g, float b)
	{
		red = r;
		green = g;
		blue = b;
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

	float cWanted::GetAlert()
	{
		return ((float*)(address + WANTED_ALERT_OFFSET))[0];
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
		//SetHidden(false);
		SetEngaging(false);
		SetSuspecting(false);
		SetAlert(0.0);
	}

	void cWanted::SetAlert(float alert)
	{
		((float*)(address + WANTED_ALERT_OFFSET))[0] = alert;
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

	Color get_rainbow(UINT offset)
	{
		UINT64 cnt = GetTickCount64();
		constexpr double freq = .0025;
		constexpr double mul = 2.0;

		Color color = Color(
			std::sin(freq * cnt + 0 + offset) * mul,
			std::sin(freq * cnt + 2 + offset) * mul,
			std::sin(freq * cnt + 4 + offset) * mul
		);
		return color;
	}

	void Tick()
	{
		t_vehicleVector vehicles = cVehicle::GetVehicles();
		for (auto& elem : vehicles)
		{
			if (elem->Rainbow)
				elem->SetColor(get_rainbow(elem->address));
		}
	}
}