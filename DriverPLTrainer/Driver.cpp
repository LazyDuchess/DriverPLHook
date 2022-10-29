#include "pch.h"
#include "Driver.h"
#include "common.h"
#include <iostream>
#include <time.h>
#include <string>

#define CHARACTER_OFFSET 0x63C
#define MODEL_OFFSET 0x744
#define MODEL_OFFSET2 0x764
#define HEALTH_OFFSET 0x684

#define PLAYER_POINTER 0x30C7A0
#define PLAYER_MONEY_OFFSET 0x884

namespace Driver {

	char* modBase;

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
		if (memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			if (PlayerSingleton != NULL)
			{
				if (PlayerSingleton->address == addr)
					return PlayerSingleton;
			}
			PlayerSingleton = new cPlayer(addr);
			return PlayerSingleton;
		}
		return NULL;
	}

	void SetModuleBase(char* moduleBase) {
		modBase = moduleBase;
		srand(time(NULL));
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
	bool cPed::IsPlayer()
	{
		cPed* play = GetPlayer();
		if (play == NULL)
			return false;
		if (play->address == address)
			return true;
		return false;
	}
	cPed* cPed::GetPlayer()
	{
		DWORD addr = (DWORD)modBase + 0x0030C6D8;
		if (memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			addr += 0x1C;
			if (memory_readable((DWORD*)addr, 4))
			{
				memcpy_s(&addr, 4, (DWORD*)addr, 4);
				if (PlayerPed != NULL)
				{
					if (PlayerPed->address == addr)
						return PlayerPed;
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
		if (memory_readable((DWORD*)addr, 4))
		{
			memcpy_s(&addr, 4, (DWORD*)addr, 4);
			addr += 0x31C;
			if (memory_readable((DWORD*)addr, 4))
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