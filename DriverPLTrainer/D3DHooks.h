#pragma once
#include <d3d9.h>

class DXGH
{
public:
	bool DrawMessage(LPD3DXFONT font, unsigned int x, unsigned int y, int alpha, unsigned char r, unsigned char g, unsigned char b, LPCSTR Message); //Draw message
	static HRESULT WINAPI h_EndScene(LPDIRECT3DDEVICE9 pDevice); //End scene
};

int StartD3DHooks(); //call this later for our hooks

typedef HRESULT(WINAPI* EndScene_t)(LPDIRECT3DDEVICE9 pDevice); //For our original
extern DXGH	DXGameHook;