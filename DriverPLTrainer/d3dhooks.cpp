#include "D3DHooks.h"
#include <d3d9.h>
#include <d3dx9.h>

DXGH	                                        DXGameHook;
LPD3DXFONT					m_font = NULL;

//Our end scene hook
HRESULT WINAPI DXGH::h_EndScene(LPDIRECT3DDEVICE9 pDevice)
{
	//First register our txt
	D3DXCreateFont(pDevice, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_font);

	DXGameHook.DrawMessage(m_font, 40, 40, 255, 255, 0, 255, "pSBadCompany2 Test 1");
	DXGameHook.DrawMessage(m_font, 40, 60, 255, 255, 255, 255, "pSBadCompany2 Test 2");
	DXGameHook.DrawMessage(m_font, 40, 80, 255, 255, 255, 0, "pSBadCompany2 Test 3");
	DXGameHook.DrawMessage(m_font, 40, 100, 255, 255, 200, 155, "pSBadCompany2 Test 4");
	DXGameHook.DrawMessage(m_font, 40, 120, 255, 200, 0, 200, "pSBadCompany2 Test 5");
	//Draw our messages ^^


	return org_EndScene(pDevice); //Return orig
}

//Now for our Drawtext funciton:

bool DXGH::DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message)
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

int StartD3DHooks()
{
	DWORD D3DPattern, * vTable, DXBase = NULL;
	DXBase = (DWORD)LoadLibraryA("d3d9.dll");

	while (!DXBase);
	{
		D3DPattern = GameHook->FindPattern(DXBase, 0x128000, (PBYTE)"\xC7\x06\x00\x00\x00\x00\x89\x86\x00\x00\x00\x00\x89\x86", "xx????xx????xx");	//Searching for the D3D Pattern
	}

	if (D3DPattern)
	{
		//if D3D Pattern is found. then do hooks
		memcpy(&vTable, (void*)(D3DTrue + 2), 4);
		org_EndScene = (EndScene_t)GameHook->DetourFunction((PBYTE)vTable[ENDSCENE], (PBYTE)DXGameHook.h_EndScene, 5);
		org_DrawIndexedPrimitive = (DrawIndexedPrimitive_t)GameHook->DetourFunction((PBYTE)vTable[DRAWINDEXEDPRIMITIVE], (PBYTE)DXGameHook.h_DrawIndexedPrimitive, 5);
	}
	return 0;
}