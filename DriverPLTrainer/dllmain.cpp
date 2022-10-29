// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "D3DHook.h"
#include <Windows.h>
#include <d3d9.h> 
#include <d3dx9.h>
#include "Driver.h"
#include <string>
#pragma comment(lib, "D3D Hook x86.lib")
#pragma comment(lib, "d3dx9.lib")

typedef long(__stdcall* tEndScene)(LPDIRECT3DDEVICE9);
tEndScene oD3D9EndScene = NULL;
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

void Draw(LPDIRECT3DDEVICE9 pDevice)
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();

	if (playerPed != NULL)
	{
		if (!madeFont)
		{
			HRESULT res = D3DXCreateFont(pDevice, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);
			if (res == S_OK)
				madeFont = true;
		}
		D3DCOLOR fontColor = D3DCOLOR_ARGB(255, 255, 255, 0);
		RECT rct; //Font
		rct.left = 40;
		rct.right = 600;
		rct.top = 40;
		rct.bottom = rct.top + 200;
		std::wstring playerInfoString = L"Player Spawned";
		playerInfoString.append(L"\nModel: ");
		playerInfoString.append(std::to_wstring(playerPed->GetModel()));
		playerInfoString.append(L"\nCharacter: ");
		playerInfoString.append(std::to_wstring(playerPed->GetCharacter()));
		if (madeFont)
			m_font->DrawText(NULL, playerInfoString.c_str(), -1, &rct, 0, fontColor);
	}

}

void Update()
{
	Driver::cPed* playerPed = Driver::cPed::GetPlayer();

	if (playerPed != NULL)
	{
		if (playerPed->GetCharacter() == CHARACTER_PLAYER_THEN || playerPed->GetModel() == MODEL_PLAYER_THEN)
		{
			Driver::Character* randomChar = Driver::Character::GetRandom();
			playerPed->SetCharacter(randomChar->character);
			playerPed->SetModel(randomChar->model);
		}

		Driver::cPed* allPeds = Driver::cPed::GetPeds();
		for (int i = 0; i < PED_AMOUNT; i++)
		{
			if (allPeds[i].address != playerPed->address)
			{
				allPeds[i].SetHealth(playerPed->GetHealth() - 50.0);
				allPeds[i].SetCharacter(CHARACTER_PLAYER_THEN);
				allPeds[i].SetModel(MODEL_PLAYER_THEN);
			}
		}
	}
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

long __stdcall hkD3D9EndScene(LPDIRECT3DDEVICE9 pDevice)
{
	Update();
	if (startDrawing && !consoleDebug)
		Draw(pDevice);
	if (consoleDebug)
		ConsoleDebug();
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

