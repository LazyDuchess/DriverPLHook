#include "pch.h"
#include "Driver.h"
#include "driver_hooks.h"
#include "hooking.h"

#define VEHICLE_CTOR 0x1AE381
#define VEHICLE_DTOR 0x1A9B66
#define VEHICLE_CRASHDAMAGE 0x19D655

namespace Driver {

	t_vehicleMap cVehicleMap = t_vehicleMap();

	char* returnCtorHook;
	char* returnDtorHook;
	char* returnCrashDamageHook;

	void _stdcall onVehicleCtor(DWORD address)
	{
		cVehicleMap[address] = new cVehicle(address);
	}

	void _stdcall onVehicleDtor(DWORD address)
	{
		delete cVehicleMap[address];
	}

	bool _stdcall hookVehicleIsCrashProof(DWORD address)
	{
		cVehicle* vehicleEx = cVehicleMap[address];
		if (vehicleEx->CrashProof)
			return true;
		return false;
	}

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
			movss xmm4, [ebp+0x0C]
			jmp returnCrashDamageHook

			TargetLabel:
			jmp crashDamageJmp;
		}
	}

	char* ctorCall;

	__declspec(naked) void ctorHook()
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
			call ctorCall
			jmp returnCtorHook
		}
	}

	__declspec(naked) void dtorHook()
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
			jmp returnDtorHook
		}
	}

	void InitializeHooks() {
		ctorCall = modBase + 0x17F431;

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CTOR, (DWORD)ctorHook, 9);
		returnCtorHook = (char*)((BYTE*)modBase + VEHICLE_CTOR + 9);

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_DTOR, (DWORD)dtorHook, 10);
		returnDtorHook = (char*)((BYTE*)modBase + VEHICLE_DTOR + 10);

		crashDamageJmp = modBase + 0x19D7EC;

		Hooking::MakeJMP((BYTE*)modBase + VEHICLE_CRASHDAMAGE, (DWORD)crashDamageHook, 5);
		returnCrashDamageHook = (char*)((BYTE*)modBase + VEHICLE_CRASHDAMAGE + 5);
	}
}