#pragma once
#include <Windows.h>
#include <vector>

#define MODEL_PLAYER_THEN 29
#define CHARACTER_PLAYER_THEN 19
#define PED_AMOUNT 48

namespace Driver {

	/// <summary>
	/// Enable or disable the player lock on/auto-aim system.
	/// </summary>
	extern bool LockOn;

	extern char* modBase;

	void Tick();

	/// <summary>
	/// Make the timescale a static value, regardless of any modifiers.
	/// </summary>
	/// <param name="timeScale">Time scale.</param>
	void ForceTimescale(float timeScale);

	/// <summary>
	/// Restore the original timescale after using ForceTimescale.
	/// </summary>
	void RestoreTimescale();

	/// <summary>
	/// Sets the base module address and initializes the API.
	/// </summary>
	/// <param name="moduleBase">DriverParallelLines.exe address in memory.</param>
	void SetModuleBase(char* moduleBase);

	class cUINotification;

	/// <summary>
	/// Check whether the minimap is enabled or disabled.
	/// </summary>
	/// <returns></returns>
	bool OverheadMapEnabled();

	/// <summary>
	/// Enable or disable HUD overlays.
	/// </summary>
	/// <param name="enable"></param>
	void EnableOverlays(bool enable);

	/// <summary>
	/// Enable or disable the minimap.
	/// </summary>
	/// <param name="enable"></param>
	void EnableOverheadMap(bool enable);

	struct Vector3 {
	public:
		float x;
		float y;
		float z;
		Vector3(float x, float y, float z);
		float Distance(Vector3 otherVec);
		Vector3 Add(Vector3 otherVec);
	};

	struct Resolution {
	public:
		int width;
		int height;
		Resolution(int width, int height);
	};

	static class cDisplay {
	public:
		/// <summary>
		/// Get the current screen resolution.
		/// </summary>
		/// <returns>A resolution struct.</returns>
		static Resolution GetResolution();
	};

	/// <summary>
	/// Displays UI notifications on the top half of the screen.
	/// </summary>
	class cUINotification {
	public:
		static cUINotification* Get();
		/// <summary>
		/// Remove any currently displayed notification.
		/// </summary>
		void Clear();
		void Show(LPCSTR text);
		void Show(LPCSTR text, float duration);
		cUINotification(DWORD addr);
	private:
		DWORD address;
	};

	class cVehicle;

	typedef std::vector<cVehicle*> t_vehicleVector;

	/// <summary>
	/// Represents a normalized RGB color.
	/// </summary>
	struct Color {
	public:
		float red;
		float green;
		float blue;
		Color(float r, float g, float b);
	};

	/// <summary>
	/// A physics-enabled (not on rails) vehicle.
	/// </summary>
	class cVehicle {
	public:
		static t_vehicleVector GetVehicles();
		void SetColor(Color color);
		Color GetColor();
		float GetDamage();
		void SetDamage(float damage);
		void Explode();
		void Repair();
		bool operator==(const cVehicle& other);
		bool operator!=(const cVehicle& other);
		cVehicle(DWORD addr);

		/// <summary>
		/// Prevents the vehicle from getting damaged from physical impacts.
		/// </summary>
		bool CrashProof = false;
		/// <summary>
		/// Makes car constantly change color.
		/// </summary>
		bool Rainbow = false;
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
		float GetWantedDelta();
		bool GetEngaging();
		bool GetHidden();
		bool GetSuspecting();
		float GetVehicle();

		void ClearWantedLevel();

		void SetSuspicionLevel(float level);
		void SetWantedLevel(float level);
		void SetWantedDelta(float delta);
		void SetEngaging(bool engaging);
		void SetHidden(bool hidden);
		void SetSuspecting(bool suspecting);
		void SetVehicle(float vehicle);

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

		void SetAllowGetOutOfVehicle(bool allow);
		bool GetAllowGetOutOfVehicle();
		void SetAllowWeapons(bool allow);
		bool GetAllowWeapons();
		void SetDamageMultiplier(float multiplier);
		float GetDamageMultiplier();

		/// <summary>
		/// Check whether the character is actually spawned in the world.
		/// </summary>
		/// <returns></returns>
		bool Spawned();
		void SetPosition(Vector3 position);
		Vector3 GetPosition();
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
		static t_pedVector GetPeds();
		//static cPed* GetPedsBuffer();
		//static cPed* GetPed(int id);
		void Damage(float damage, bool unk);
		cPed(DWORD addr);
		cPed();
		bool operator==(const cPed& other);
		bool operator!=(const cPed& other);
		/// <summary>
		/// If set to true, game will handle death events as normal (Player death will trigger slomo and game over menu, enemy deaths will remove blips from maps, etc.)
		/// Can be set to false if you wish to handle these situations yourself for any reason.
		/// </summary>
		bool HandleDeath = true;
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