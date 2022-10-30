// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "D3DHook.h"
#include <Windows.h>
#include <d3d9.h> 
#include <d3dx9.h>
#include "Driver.h"
#include "driver_hooks.h"
#include <string>
#include <iomanip>
#include <sstream>
#include <vector>
#pragma comment(lib, "D3D Hook x86.lib")
#pragma comment(lib, "d3dx9.lib")

typedef long(__stdcall* tEndScene)(LPDIRECT3DDEVICE9);
typedef long(__stdcall* tReset)(LPDIRECT3DDEVICE9);
tEndScene oD3D9EndScene = NULL;
tReset oD3D9Reset = NULL;
LPD3DXFONT m_font = NULL;

HMODULE g_Module;
bool bExit = false;

void DrawFilledRect(int x, int y, int w, int h, D3DCOLOR color, IDirect3DDevice9* dev)
{
	D3DRECT BarRect = { x, y, x + w, y + h };
	dev->Clear(1, &BarRect, D3DCLEAR_TARGET | D3DCLEAR_TARGET, color, 0, 0);
}

bool madeFont = false;
bool menuOpen = false;

std::wstring GetWStringForBool(bool value)
{
	return value ? L"ON" : L"OFF";
}

void DrawTxt(LPD3DXFONT font, LPCWSTR string, D3DCOLOR fontColor, int left, int top)
{
	RECT rct; //Font
	rct.left = left;
	rct.right = 1920;
	rct.top = top;
	rct.bottom = rct.top + 1080;
	font->DrawTextW(NULL, string, -1, &rct, 0, fontColor);
}

void DrawTxtShadow(LPD3DXFONT font, LPCWSTR string, D3DCOLOR fontColor, int left, int top)
{
	D3DCOLOR fontBackColor = D3DCOLOR_ARGB(255, 0, 0, 0);
	int shadowOffset = 3;
	//Shadows

	DrawTxt(m_font, string, fontBackColor, left - shadowOffset, top);
	DrawTxt(m_font, string, fontBackColor, left + shadowOffset, top);

	DrawTxt(m_font, string, fontBackColor, left, top - shadowOffset);
	DrawTxt(m_font, string, fontBackColor, left, top + shadowOffset);

	DrawTxt(m_font, string, fontBackColor, left - shadowOffset, top - shadowOffset);
	DrawTxt(m_font, string, fontBackColor, left + shadowOffset, top + shadowOffset);

	DrawTxt(m_font, string, fontBackColor, left - shadowOffset, top + shadowOffset);
	DrawTxt(m_font, string, fontBackColor, left + shadowOffset, top - shadowOffset);

	//Top

	DrawTxt(m_font, string, fontColor, left, top);
}

bool neverWanted = false;
bool neverDie = false;
bool indestructibleCar = false;
bool infiniteMoney = false;

void Draw(LPDIRECT3DDEVICE9 pDevice)
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();
	if (playerPed == NULL)
		return;
	if (GetAsyncKeyState(VK_F10) & 0x01)
	{
		if (m_font != NULL)
		{
			m_font->Release();
			m_font = NULL;
		}
		madeFont = false;
		menuOpen = !menuOpen;
	}
	if (!menuOpen)
		return;
	
	if (!madeFont)
	{
		HRESULT res = D3DXCreateFont(pDevice, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
		if (res == S_OK)
			madeFont = true;
	}

	//never wanted
	if (GetAsyncKeyState(VK_NUMPAD1) & 0x01)
	{
		neverWanted = !neverWanted;
	}

	//never die
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x01)
	{
		neverDie = !neverDie;
	}

	//indestructible car
	if (GetAsyncKeyState(VK_NUMPAD3) & 0x01)
	{
		indestructibleCar = !indestructibleCar;
	}

	//infinite money
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x01)
	{
		infiniteMoney = !infiniteMoney;
	}

	//clear wanted
	if (GetAsyncKeyState(VK_NUMPAD5) & 0x01)
	{
		Driver::cWanted* pWanted = Driver::cWanted::Get();
		pWanted->ClearWantedLevel();
	}

	//kill everyone
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x01)
	{
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			Driver::cPed* ped = Driver::cPed::GetPed(i);
			if (ped != playerPed)
				ped->Damage(10.0, false);
		}
	}

	//wreck ped cars
	if (GetAsyncKeyState(VK_NUMPAD7) & 0x01)
	{
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			Driver::cPed* ped = Driver::cPed::GetPed(i);
			if (ped != playerPed && ped->InVehicle())
			{
				ped->GetVehicle()->SetDamage(100.0);
			}
		}
	}

	std::wstring togglesStr = L"Toggles:\n\n";

	togglesStr.append(L"[Numpad 1] Never Wanted: ");
	togglesStr.append(GetWStringForBool(neverWanted));
	togglesStr.append(L"\n");

	togglesStr.append(L"[Numpad 2] Never Die: ");
	togglesStr.append(GetWStringForBool(neverDie));
	togglesStr.append(L"\n");

	togglesStr.append(L"[Numpad 3] Indestructible Player Car: ");
	togglesStr.append(GetWStringForBool(indestructibleCar));
	togglesStr.append(L"\n");

	togglesStr.append(L"[Numpad 4] Infinite Money: ");
	togglesStr.append(GetWStringForBool(infiniteMoney));
	togglesStr.append(L"\n");

	std::wstring actionsStr = L"Actions:\n\n";

	actionsStr.append(L"[Numpad 5] Clear Wanted");
	actionsStr.append(L"\n");

	actionsStr.append(L"[Numpad 6] Kill Everyone");
	actionsStr.append(L"\n");

	actionsStr.append(L"[Numpad 7] Wreck Ped Cars");
	actionsStr.append(L"\n");

	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 255);

	int textX = 20;
	int textY = 20;
	if (madeFont)
	{
		DrawTxtShadow(m_font, togglesStr.c_str(), fontColor, textX, textY);
		textX += 400;
		DrawTxtShadow(m_font, actionsStr.c_str(), fontColor, textX, textY);
	}
}

void Update()
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();
	Driver::cPlayer* playerData = Driver::cPlayer::Get();
	Driver::cWanted* wantedLevel = Driver::cWanted::Get();

	if (playerPed != NULL)
	{
		//No car damage
		if (playerPed->InVehicle())
		{
			Driver::cVehicle* playerCar = playerPed->GetVehicle();
			if (indestructibleCar)
			{
				playerCar->CrashProof = true;
				playerCar->SetDamage(0.0);
			}
			else
			{
				playerCar->CrashProof = false;
			}
		}

		if (neverWanted)
		{
			//No cops
			if (wantedLevel != NULL)
				wantedLevel->ClearWantedLevel();
		}

		if (infiniteMoney)
		{
			//Force money to 999999 constantly
			if (playerData != NULL)
				playerData->SetMoney(999999);
		}

		if (neverDie)
		{
			//No dying
			playerPed->SetHealth(2.0);
		}
	}
}

long _stdcall hkD3D9Reset(LPDIRECT3DDEVICE9 pDevice)
{
	if (m_font != NULL)
	{
		m_font->Release();
		m_font = NULL;
		madeFont = false;
	}
	return oD3D9Reset(pDevice);
}

long __stdcall hkD3D9EndScene(LPDIRECT3DDEVICE9 pDevice)
{
	Update();
	Draw(pDevice);
	return oD3D9EndScene(pDevice);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
		Driver::SetModuleBase((char*)GetModuleHandleA(NULL));
		g_Module = hModule;
		DisableThreadLibraryCalls(g_Module);
		CreateThread(nullptr, 0, [](LPVOID) -> DWORD
			{
				bool d3dInitialized = false;
				while (d3dInitialized == false)
				{
					d3dInitialized = init_D3D();
				}

				
			    methodesHook(42, hkD3D9EndScene, (LPVOID*)&oD3D9EndScene); // hook endscene
				methodesHook(16, hkD3D9Reset, (LPVOID*)&oD3D9Reset);

				while (!bExit)
				{
					Sleep(100); // Sleeps until shutdown
				}

				methodesUnhook(); // disables and removes all hooks

				FreeLibraryAndExitThread(g_Module, 0);
			}, nullptr, 0, nullptr);
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

