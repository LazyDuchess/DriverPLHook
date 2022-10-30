#pragma once
#include <Windows.h>
#include <vector>

#define MODEL_PLAYER_THEN 29
#define CHARACTER_PLAYER_THEN 19
#define PED_AMOUNT 48

namespace Driver {

	extern char* modBase;

	/// <summary>
	/// Sets the base module address and initializes the API.
	/// </summary>
	/// <param name="moduleBase">DriverParallelLines.exe address in memory.</param>
	void SetModuleBase(char* moduleBase);

	class cVehicle;

	typedef std::vector<cVehicle*> t_vehicleVector;

	/// <summary>
	/// A vehicle.
	/// </summary>
	class cVehicle {
	public:
		static t_vehicleVector GetVehicles();
		float GetDamage();
		void SetDamage(float damage);
		cVehicle(DWORD addr);

		/// <summary>
		/// Prevents the vehicle from getting damaged from physical impacts.
		/// </summary>
		bool CrashProof = false;
		DWORD address;
	};

	/// <summary>
	/// Wanted level for player.
	/// </summary>
	class cWanted {
	public:
		static cWanted* Get();

		float GetSuspicionLevel();
		float GetWantedLevel();
		bool GetEngaging();
		bool GetHidden();
		bool GetSuspecting();
		float GetAlert();

		void ClearWantedLevel();

		void SetSuspicionLevel(float level);
		void SetWantedLevel(float level);
		void SetEngaging(bool engaging);
		void SetHidden(bool hidden);
		void SetSuspecting(bool suspecting);
		void SetAlert(float alert);

		cWanted(DWORD addr);
	private:
		DWORD address;
	};

	class cPed;

	typedef std::vector<cPed*> t_pedVector;

	/// <summary>
	/// Pedestrian.
	/// </summary>
	class cPed {
	public:
		float GetHealth();
		bool InVehicle();
		cVehicle* GetVehicle();
		DWORD GetVehiclePointer();
		void SetHealth(float health);
		void SetModel(int model);
		void SetCharacter(int character);
		int GetModel();
		int GetCharacter();
		static cPed* GetPlayer();
		//static t_pedVector GetPeds();
		//static cPed* GetPedsBuffer();
		static cPed* GetPed(int id);
		void Damage(float damage);
		cPed(DWORD addr);
		cPed();
		bool operator==(const cPed& other);
		bool operator!=(const cPed& other);
		DWORD address;
	};

	/// <summary>
	/// Player data.
	/// </summary>
	class cPlayer {
	public:
		static cPlayer* Get();
		int GetMoney();
		void SetMoney(int money);
		cPlayer(DWORD addr);
	private:
		DWORD address;
	};
	
	/// <summary>
	/// Character appearance.
	/// </summary>
	class Character {
	public:
		int model;
		int character;
		Character(int model, int character);
		static Character* GetRandom();
	};

	extern Character* characters[];
	
}