#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"

#define VEHICLE_CTOR 0x1AE381
#define VEHICLE_DTOR 0x1A9B66
#define VEHICLE_CRASHDAMAGE 0x19D655 //stopped using for now
#define VEHICLE_CRASH 0x1AA318

#define PED_CTOR 0xEB6FF
#define PED_DTOR 0xEB8C2

#define GAME_EXIT 0x20269D

#define UI_NOTIF_CTOR 0x72E6C
#define UI_NOTIF_MOV_ADDR 0x2431D0

#define HANDLEDEATH_HOOK_OFFSET 0xE7DD7

#define UI_MINIMAP_DRAW_HOOK_OFFSET 0xB9F0F

#define GENERAL_UI_DRAW_HOOK_OFFSET 0x14FC40
#define GENERAL_UI_DRAW_HOOK_JNE_OFFSET 0x14FC32

namespace Driver {
	char* GeneralUIJNE;
	char* GeneralUIReturnDrawHook;
	//Replacing 6 bytes in this one. Hook to skip drawing most UI if UI disabled.
	__declspec(naked) void GeneralUIDrawHook()
	{
		__asm {
			jne ElseLabel
			jmp OutLabel

			OutLabel:
				mov edi, ebx
				lea esi,[ebp+0x0C]
				jmp GeneralUIReturnDrawHook

			ElseLabel:
				cmp DrawHUD, 0
				jne NotEqualLabel
				jmp OutLabel

			NotEqualLabel:
				jmp GeneralUIJNE
		}
	}

	char* cUIMinimapReturnDrawHook;
	//Replacing 6 bytes in this one. Hook to skip drawing minimap if UI disabled.
	__declspec(naked) void cUIMinimapDrawHook()
	{
		__asm {
			cmp DrawHUD, 0
			je TrueLabel
			mov eax,[ebp+0x08]
			cmp eax,[ebx+0x34]
			jmp cUIMinimapReturnDrawHook

			TrueLabel:
			pop edi
				pop esi
				pop ebx
				mov esp, ebp
				pop ebp
				ret 0x0004
		}
	}

	cUINotification* cUINotifSingleton;

	cUINotification* cUINotification::Get()
	{
		return cUINotifSingleton;
	}

	void _stdcall onUINotifCtor(DWORD address)
	{
		cUINotifSingleton = new cUINotification(address);
	}

	bool _stdcall hookShouldHandleDeath(DWORD address)
	{
		cPed* pedEx = cPedMap[address];
		if (!pedEx->HandleDeath)
			return false;
		return true;
	}

	//Replacing 9 bytes in this one, all the way to the end.
	__declspec(naked) void cPedDeathGameOverHook()
	{
		__asm {
			jp Parity
			push ecx
			call hookShouldHandleDeath
			cmp al, 0x1
			jne Parity
			xor eax, eax
			inc eax
			ret

			Parity:
			xor eax, eax
			ret
		}
	}

	char* cUINotificationReturnCtorHook;
	char* cUINotificationMovAddr;

	__declspec(naked) void cUINotifCtorHook()
	{
		__asm {
			push edi
			push eax
			push edx
			push ecx
			push ebx
			push esi

			push ecx
			call onUINotifCtor

			pop esi
			pop ebx
			pop ecx
			pop edx
			pop eax
			pop edi

			push esi
			mov esi, ecx
			push eax
			mov eax, cUINotificationMovAddr
			mov[esi + 0x4], eax
			pop eax
			jmp cUINotificationReturnCtorHook
		}
	}

	t_vehicleMap cVehicleMap = t_vehicleMap();
	t_pedMap cPedMap = t_pedMap();

	char* cPedReturnCtorHook;
	char* cPedReturnDtorHook;

	char* cVehiclereturnCtorHook;
	char* cVehiclereturnDtorHook;
	//char* returnCrashDamageHook;
	char* returnCrashHook;

	void _stdcall exitGame()
	{
		exit(0);
	}

	void _stdcall onPedCtor(DWORD address)
	{
		cPedMap[address] = new cPed(address);
	}

	void _stdcall onPedDtor(DWORD address)
	{
		delete cPedMap[address];
		cPedMap.erase(address);
	}

	void _stdcall onVehicleCtor(DWORD address)
	{
		cVehicleMap[address] = new cVehicle(address);
	}

	void _stdcall onVehicleDtor(DWORD address)
	{
		delete cVehicleMap[address];
		cVehicleMap.erase(address);
	}

	bool _stdcall hookVehicleIsCrashProof(DWORD address)
	{
		cVehicle* vehicleEx = cVehicleMap[address];
		if (vehicleEx->CrashProof)
			return true;
		return false;
	}

	char* crashJmp;

	_declspec(naked) void exitHook()
	{
		__asm {
			call exitGame
			ret
		}
	}

	/// <summary>
	/// Hooks HandleCrash function to add cVehicle.CrashProof functionality.
	/// </summary>
	_declspec(naked) void crashHook()
	{
		__asm {
			push edi
			push eax
			push edx
			push ecx
			push ebx

			push ecx
			call hookVehicleIsCrashProof
			test al, al

			pop ebx
			pop ecx
			pop edx
			pop eax
			pop edi

			jnz TargetLabel
			cmp dword ptr[edi + 0x90], 0x1A
			jmp returnCrashHook

			TargetLabel :
			mov ebx, ecx
			jmp crashJmp;
		}
	}
	/*
	char* crashDamageJmp;

	_declspec(naked) void crashDamageHook()
	{
		__asm {
			push eax
			push edx
			push ecx
			push ebx

			push ebx
			call hookVehicleIsCrashProof
			test al, al

			pop ebx
			pop ecx
			pop edx
			pop eax

			jnz TargetLabel
			movss xmm4, [ebp + 0x0C]
			jmp returnCrashDamageHook

			TargetLabel :
			jmp handleCrashJmp;
		}
	}*/

	char* cVehiclectorCall;

	/// <summary>
	/// cVehicle constructor hook.
	/// </summary>
	__declspec(naked) void cVehiclectorHook()
	{
		__asm {
			push eax
			push edx
			push ecx

			push ecx
			call onVehicleCtor

			pop ecx
			pop edx
			pop eax

			push ebx
			push esi
			mov esi, ecx
			call cVehiclectorCall
			jmp cVehiclereturnCtorHook
		}
	}

	/// <summary>
	/// cVehicle destructor hook.
	/// </summary>
	__declspec(naked) void cVehicledtorHook()
	{
		__asm {
			push eax
			push edx
			push ecx

			push ecx
			call onVehicleDtor
			
			pop ecx
			pop edx
			pop eax
			
			push esi
			mov esi, ecx
			cmp byte ptr[esi + 0x556], 0x00
			jmp cVehiclereturnDtorHook
		}
	}

	_declspec(naked) void cPedCtorHook()
	{
		__asm {
			push eax
			push edx
			push ecx
			push ebp
			push ebx
			push esi

			push ecx
			call onPedCtor

			pop esi
			pop ebx
			pop ebp
			pop ecx
			pop edx
			pop eax

			push ebp
			mov ebp, esp
			sub esp, 0x10
			jmp cPedReturnCtorHook
		}
	}

	__declspec(naked) void cPedDtorHook()
	{
		__asm {
			push eax
			push edx
			push ecx
			push ebp
			push ebx
			push esi

			push ecx
			call onPedDtor

			pop esi
			pop ebx
			pop ebp
			pop ecx
			pop edx
			pop eax

			push ecx
			push esi
			mov esi, ecx
			push edi
			jmp cPedReturnDtorHook
		}
	}

	/// <summary>
	/// Hooks game code to construct and destruct the wrappers when the actual native classes are constructed and destructed.
	/// Also to add extra functionality.
	/// </summary>
	void InitializeHooks() {
		cVehiclectorCall = modBase + 0x17F431;

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CTOR, (DWORD)cVehiclectorHook, 9);
		cVehiclereturnCtorHook = (char*)((BYTE*)modBase + VEHICLE_CTOR + 9);

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_DTOR, (DWORD)cVehicledtorHook, 10);
		cVehiclereturnDtorHook = (char*)((BYTE*)modBase + VEHICLE_DTOR + 10);

		crashJmp = modBase + 0x1AB77E;
		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CRASH, (DWORD)crashHook, 7);
		returnCrashHook = (char*)((BYTE*)modBase + VEHICLE_CRASH + 7);

		Hooking::MakeJMP((BYTE*)modBase + PED_CTOR, (DWORD)cPedCtorHook, 6);
		cPedReturnCtorHook = (char*)((BYTE*)modBase + PED_CTOR + 6);

		Hooking::MakeJMP((BYTE*)modBase + PED_DTOR, (DWORD)cPedDtorHook, 5);
		cPedReturnDtorHook = (char*)((BYTE*)modBase + PED_DTOR + 5);

		//exit: 7

		Hooking::MakeJMP((BYTE*)modBase + GAME_EXIT, (DWORD)exitHook, 7);

		Hooking::MakeJMP((BYTE*)modBase + UI_NOTIF_CTOR, (DWORD)cUINotifCtorHook, 10);
		cUINotificationMovAddr = (char*)((BYTE*)modBase + UI_NOTIF_MOV_ADDR);
		cUINotificationReturnCtorHook = (char*)((BYTE*)modBase + UI_NOTIF_CTOR + 10);
		
		Hooking::MakeJMP((BYTE*)modBase + HANDLEDEATH_HOOK_OFFSET, (DWORD)cPedDeathGameOverHook, 9);

		Hooking::MakeJMP((BYTE*)modBase + UI_MINIMAP_DRAW_HOOK_OFFSET, (DWORD)cUIMinimapDrawHook, 6);
		cUIMinimapReturnDrawHook = (char*)((BYTE*)modBase + UI_MINIMAP_DRAW_HOOK_OFFSET + 6);
		
		Hooking::MakeJMP((BYTE*)modBase + GENERAL_UI_DRAW_HOOK_OFFSET, (DWORD)GeneralUIDrawHook, 7);
		GeneralUIReturnDrawHook = (char*)((BYTE*)modBase + GENERAL_UI_DRAW_HOOK_OFFSET + 7);
		GeneralUIJNE = (char*)((BYTE*)modBase + GENERAL_UI_DRAW_HOOK_JNE_OFFSET);
		/*
		crashDamageJmp = modBase + 0x19D7EC;

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CRASHDAMAGE, (DWORD)crashDamageHook, 5);
		returnCrashDamageHook = (char*)((BYTE*)modBase + VEHICLE_CRASHDAMAGE + 5);*/
	}
}