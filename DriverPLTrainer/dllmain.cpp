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

bool DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message)
{	// Create a colour for the text
	D3DCOLOR fontColor = D3DCOLOR_ARGB(alpha, r, g, b);
	RECT rct; //Font
	rct.left = x;
	rct.right = 1680;
	rct.top = y;
	rct.bottom = rct.top + 200;
	font->DrawTextA(NULL, Message, -1, &rct, 0, fontColor);
	return true;
}

bool madeFont = false;
bool startDrawing = false;
bool consoleDebug = true;

bool debugMode = false;

void Draw(LPDIRECT3DDEVICE9 pDevice)
{
	if (GetAsyncKeyState(VK_F10) & 0x01)
	{
		if (m_font != NULL)
		{
			m_font->Release();
			m_font = NULL;
		}
		madeFont = false;
		debugMode = !debugMode;
	}
	if (!debugMode)
		return;
	if (!madeFont)
	{
		HRESULT res = D3DXCreateFont(pDevice, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
		if (res == S_OK)
			madeFont = true;
	}
	D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 0);
	RECT rct; //Font
	rct.left = 40;
	rct.right = 1920;
	rct.top = 40;
	rct.bottom = rct.top + 1080;

	std::wstring debugStr = L"Ped debug: ";

	Driver::cPed* player = Driver::cPed::GetPlayer();

	for (int i = 0; i < PED_AMOUNT; i++)
	{
		Driver::cPed* ped = Driver::cPed::GetPed(i);
		debugStr.append(L"\n");
		debugStr.append(std::to_wstring(ped->address));
		if (ped->address != player->address)
		{
			debugStr.append(L" - not the player");
			ped->Damage(1.0);
		}
		if (ped->InVehicle())
			debugStr.append(L" - in a vehicle.");
	}

	/*
	std::wstring debugStr = L"Ped vehicle debug: ";
	Driver::t_pedVector peds = Driver::cPed::GetPeds();
	for (Driver::cPed* elem : peds)
	{
		if (elem->InVehicle())
		{
			debugStr.append(L"\n");
			debugStr.append(std::to_wstring(elem->address));
			debugStr.append(L" in vehicle ");
			debugStr.append(std::to_wstring(elem->GetVehicle()->address));
		}
	}*/
	/*
	for (int i = 0; i < PED_AMOUNT; i++)
	{
		if (peds[i] != NULL)
		{
			if (peds[i].address != NULL)
			{
				
				if (peds[i].InVehicle())
				{
					debugStr.append(L"\n");
					debugStr.append(std::to_wstring(i));
				}
			}
		}
	}*/
	if (madeFont)
		m_font->DrawText(NULL, debugStr.c_str(), -1, &rct, 0, fontColor);
}

int lastModel = -1;

void UpdateBodySnatcher()
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();
	Driver::cWanted* wantedLevel = Driver::cWanted::Get();
	if (playerPed != NULL)
	{
		if (lastModel == -1)
			lastModel = playerPed->GetModel();
		if (wantedLevel != NULL)
		{
			if (!wantedLevel->GetEngaging() && playerPed->GetModel() != lastModel)
			{
				wantedLevel->ClearWantedLevel();
			}
		}
		lastModel = playerPed->GetModel();
	}
}

void Update()
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();
	Driver::cPlayer* playerData = Driver::cPlayer::Get();
	Driver::cWanted* wantedLevel = Driver::cWanted::Get();

	

	if (playerPed != NULL)
	{
		/*
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			Driver::cPed* ped = Driver::cPed::GetPed(i);
			ped->Damage(0.1);
		}*/

		//No car damage
		if (playerPed->InVehicle())
		{
			Driver::cVehicle* playerCar = playerPed->GetVehicle();
			playerCar->CrashProof = true;
			playerCar->SetDamage(0.0);
		}

		//No cops
		if (wantedLevel != NULL)
			wantedLevel->ClearWantedLevel();

		//Force money to 999999 constantly
		if (playerData != NULL)
			playerData->SetMoney(999999);

		//No dying
		//playerPed->SetHealth(999);

		

		

		/*
		Driver::t_pedVector peds = Driver::cPed::GetPeds();

		float targetHealth = playerPed->GetHealth() - 0.5;

		for (const auto& elem : peds)
		{
			elem->Damage(-1.0);
		}*/

		//Dunno, just testing. Don't question it.
		/*
		Driver::cPed* allPeds = Driver::cPed::GetPeds();
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			if (allPeds[i] != *playerPed && allPeds[i].InVehicle() && allPeds[i].GetHealth() > 0.01)
			{
				allPeds[i].SetHealth(0.01);
			}
		}*/

		/*
		//Give ourselves a random skin
		if (playerPed->GetCharacter() == CHARACTER_PLAYER_THEN || playerPed->GetModel() == MODEL_PLAYER_THEN)
		{
			Driver::Character* randomChar = Driver::Character::GetRandom();
			playerPed->SetCharacter(randomChar->character);
			playerPed->SetModel(randomChar->model);
		}

		//Turn all peds into TK, and give them our HP minus 50.
		Driver::cPed* allPeds = Driver::cPed::GetPeds();
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			if (allPeds[i] != *playerPed)
			{
				allPeds[i].SetHealth(playerPed->GetHealth() - 0.5);
				allPeds[i].SetCharacter(CHARACTER_PLAYER_THEN);
				allPeds[i].SetModel(MODEL_PLAYER_THEN);
			}
		}*/
	}

	

	//Input::Tick();
}

void ConsoleDebug()
{
	/*
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();

	if (playerPed != NULL)
	{
		std::wstring playerInfoString = L"\nPlayer Spawned";
		playerInfoString.append(L"\nModel: ");
		playerInfoString.append(std::to_wstring(playerPed->GetModel()));
		playerInfoString.append(L"\nCharacter: ");
		playerInfoString.append(std::to_wstring(playerPed->GetCharacter()));
		wprintf(playerInfoString.c_str());
	}*/
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
	if (startDrawing)
		Draw(pDevice);
	if (consoleDebug)
		ConsoleDebug();
	return oD3D9EndScene(pDevice);
}

WNDPROC oWndProc = NULL;


LRESULT APIENTRY WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (debugMode)
		return false;
	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
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
		if (consoleDebug)
		{
			AllocConsole();
			freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		}
		Driver::SetModuleBase((char*)GetModuleHandleA(NULL));
		g_Module = hModule;
		DisableThreadLibraryCalls(g_Module);
		CreateThread(nullptr, 0, [](LPVOID)->DWORD
		{
			Sleep(5000);
			startDrawing = true;
			ExitThread(0);
		}, nullptr, 0, nullptr);

		CreateThread(nullptr, 0, [](LPVOID) -> DWORD
			{
				bool d3dInitialized = false;
				while (d3dInitialized == false)
				{
					d3dInitialized = init_D3D();
				}

				
			    methodesHook(42, hkD3D9EndScene, (LPVOID*)&oD3D9EndScene); // hook endscene
				methodesHook(16, hkD3D9Reset, (LPVOID*)&oD3D9Reset);

				//Wndproc hook doesn't work very well sadly

				/*
				Sleep(2000);
				wndProcHook("Driver: Parallel Lines", (LONG_PTR)WndProc, oWndProc); // wndproc hook*/

				while (!bExit)
				{
					Sleep(100); // Sleeps until shutdown
				}

				methodesUnhook(); // disables and removes all hooks
				wndProcUnhook(oWndProc); // removes wndproc hook

				FreeLibraryAndExitThread(g_Module, 0);
			}, nullptr, 0, nullptr);
		break;
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

