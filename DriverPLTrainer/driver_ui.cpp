#include "pch.h"
#include "Driver.h"
#include "hooking.h"
#define UI_NOTIFY_OFFSET 0x72C34
//#define UI_NOTIF2 0x72E48
#define UI_CLEAR 0x72DA2

#define CUSTOM_STRING_OFFSET 0x2445CC
#define STRING_MAX_LENGTH 255
#define CUSTOM_STRING_NOTIF_HOOK_OFFSET 0x72B63

namespace Driver {

	//The way I'm getting this to work atm is a bit of a smelly hack, but shouldn't cause any issues.
	char customString[STRING_MAX_LENGTH];

	cUINotification::cUINotification(DWORD addr)
	{
		address = addr;
	}

	char* notifClearAddr;

	__declspec(naked) void callUINotifClear(DWORD address)
	{
		__asm {
			mov ecx, [esp + 0x4]
			add ecx, 0xC
			call notifClearAddr
			ret
		}
	}
	/*
	char* notifTest2Addr;
	
	__declspec(naked) void callUINotifTest2(DWORD address)
	{
		__asm {
			mov ecx, [esp + 0x4]
			mov ebp, esp
			lea eax,[ebp-0x1C]
			push eax
			mov esi, ecx
			lea ecx, [esi+0x0C]
			call notifTest2Addr
			ret
		}
	}*/

	char* notifyAddr;

	__declspec(naked) void callUINotifTest(DWORD address, float duration)
	{
		__asm {
			//pull address param
			mov ecx, [esp + 0x4]
			mov ebx, [esp + 0x8]
			mov esi, ebx
			add esi, 0x00000001
			/*push 0x0019FAA8
			push 0x0116D248
			push 0xFFFFFFFE
			push 0x171F2260
			push 0x00486E0D
			push 0x1416D248*/

			push esi
			push ebx
			//push 0x40A00001
			//push 0x40A00000
			push 0x10000000
			push 0x00000000
			push 0x00000000
			call notifyAddr
			ret
		}
	}

	void cUINotification::Clear()
	{
		notifClearAddr = modBase + UI_CLEAR;
		callUINotifClear(address);
	}

	void HookCustomString()
	{
		char* pCustomString = customString;
		Hooking::WriteToMemory((DWORD)modBase + CUSTOM_STRING_NOTIF_HOOK_OFFSET, &pCustomString, 4);
	}

	void cUINotification::Show(LPCSTR text)
	{
		Show(text, 5.0);
	}

	void cUINotification::Show(LPCSTR text, float duration)
	{
		notifyAddr = modBase + UI_NOTIFY_OFFSET;
		Clear();
		ZeroMemory(customString, STRING_MAX_LENGTH);
		memcpy_s(customString, STRING_MAX_LENGTH, text, strlen(text));
		HookCustomString();
		callUINotifTest(address, duration);
	}
}