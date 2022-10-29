#pragma once
#include <Windows.h>

#define MODEL_PLAYER_THEN 29
#define CHARACTER_PLAYER_THEN 19
#define PED_AMOUNT 48

namespace Driver {

	extern char* modBase;

	void SetModuleBase(char* moduleBase);

	class cVehicle {
	public:
		float GetDamage();
		void SetDamage(float damage);
		cVehicle(DWORD addr);

		/// <summary>
		/// Prevents the vehicle from getting damaged from physical impacts.
		/// </summary>
		bool CrashProof = false;

	private:
		DWORD address;
	};

	class cWanted {
	public:
		static cWanted* Get();

		float GetSuspicionLevel();
		float GetWantedLevel();
		bool GetEngaging();
		bool GetHidden();
		bool GetSuspecting();

		void ClearWantedLevel();

		void SetSuspicionLevel(float level);
		void SetWantedLevel(float level);
		void SetEngaging(bool engaging);
		void SetHidden(bool hidden);
		void SetSuspecting(bool suspecting);

		cWanted(DWORD addr);
	private:
		DWORD address;
	};

	class cPed {
	public:
		float GetHealth();
		bool InVehicle();
		cVehicle* GetVehicle();
		void SetHealth(float health);
		void SetModel(int model);
		void SetCharacter(int character);
		int GetModel();
		int GetCharacter();
		static cPed* GetPlayer();
		static cPed* GetPeds();
		cPed(DWORD addr);
		cPed();
		bool operator==(const cPed& other);
		bool operator!=(const cPed& other);
	private:
		DWORD address;
	};

	class cPlayer {
	public:
		static cPlayer* Get();
		int GetMoney();
		void SetMoney(int money);
		cPlayer(DWORD addr);
	private:
		DWORD address;
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