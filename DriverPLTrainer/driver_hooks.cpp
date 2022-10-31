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

namespace Driver {

	t_vehicleMap cVehicleMap = t_vehicleMap();
	t_pedMap cPedMap = t_pedMap();

	char* cPedReturnCtorHook;
	char* cPedReturnDtorHook;

	char* cVehiclereturnCtorHook;
	char* cVehiclereturnDtorHook;
	//char* returnCrashDamageHook;
	char* returnCrashHook;

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
		
		/*
		crashDamageJmp = modBase + 0x19D7EC;

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CRASHDAMAGE, (DWORD)crashDamageHook, 5);
		returnCrashDamageHook = (char*)((BYTE*)modBase + VEHICLE_CRASHDAMAGE + 5);*/
	}
}