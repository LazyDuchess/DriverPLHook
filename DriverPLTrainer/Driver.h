#pragma once
#include <Windows.h>

#define MODEL_PLAYER_THEN 29
#define CHARACTER_PLAYER_THEN 19
#define PED_AMOUNT 48

namespace Driver {
	void SetModuleBase(char* moduleBase);
	class cPed {
	public:
		float GetHealth();
		void SetHealth(float health);
		void SetModel(int model);
		void SetCharacter(int character);
		int GetModel();
		int GetCharacter();
		static cPed* GetPlayer();
		bool IsPlayer();
		static cPed* GetPeds();
		DWORD address;
		cPed(DWORD addr);
		cPed();
	};

	class cPlayer {
	public:
		static cPlayer* Get();
		int GetMoney();
		void SetMoney(int money);
		DWORD address;
		cPlayer(DWORD addr);
	};
	
	class Character {
	public:
		int model;
		int character;
		Character(int model, int character);
		static Character* GetRandom();
	};

	extern Character* characters[];
}