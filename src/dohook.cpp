#include "dohook.h"
#include <language.h>
#include <tibsun/tibsun_globals.h>
#include <scenario.h>

static std::map<UINT, std::string> g_mapFetchStr;

decltype(H_Fetch_String) H_Fetch_String;
const char* Re_Fetch_String(UINT uID)
{
	if (uID == 13)
		int a = 0;
	auto iter = g_mapFetchStr.find(uID);
	if (iter != g_mapFetchStr.end())
		return iter->second.c_str();
	auto str = H_Fetch_String(uID);
	unix_line_feed(const_cast<char*>(str));
	auto ret = g_mapFetchStr.insert({ uID, to_u8local(str) });
	if (ret.second)
		return ret.first->second.c_str();
	return "STRING FAILED";
}

static struct hook_guard
{
	inline ~hook_guard() {
		if(hhook) UnhookWindowsHookEx(hhook);
	}

	HHOOK hhook = nullptr;
} hg;
int g_cliX = 0, g_cliY = 0;
LRESULT CALLBACK WndProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	auto the = (CWPRETSTRUCT*)lParam;
	if (the->hwnd == MainWindow)
	{
		switch (the->message) {
		case WM_MOVE:
			g_cliX = LOWORD(the->lParam);
			g_cliY = HIWORD(the->lParam);
			break;
		}
	}
	return CallNextHookEx(hg.hhook, nCode, wParam, lParam);
}

static HWND g_MainWindow;
decltype(H_ShowWindow) H_ShowWindow;
BOOL CALLBACK Re_ShowWindow(HWND hWnd, int nCmdShow)
{
	auto ret = H_ShowWindow(hWnd, nCmdShow);
	if (hWnd == MainWindow)
	{
		g_MainWindow = MainWindow;
		POINT pt{ 0,0 };
		ClientToScreen(hWnd,&pt);
		g_cliX = pt.x;
		g_cliY = pt.y;
		hg.hhook = SetWindowsHookEx(WH_CALLWNDPROCRET, WndProc, 0, GetWindowThreadProcessId(hWnd, NULL));

		char text[256];
		GetWindowTextA(hWnd, text, 256);
		auto u8 = to_u8local(text);
		std::string ansi;
		u8_to_ansi(u8.c_str(), ansi);
		SetWindowTextA(hWnd, ansi.c_str());
	}
	return ret;
}

decltype(H_Draw_Dlg_Text) H_Draw_Dlg_Text;
int __fastcall Re_Draw_Dlg_Text(Surface* pSur, const char* lpText, RECT* pRect, const char* aFontName, COLORREF color, char nStyle, int nX)
{
	using AL = MyFont::Align;

	auto u8 = g_text.u8(lpText);

	std::u32string u32str;
	to_u32str(u8, u32str);
	auto chrs = g_fnt_ui.cs(u32str);
	auto& opts = g_fnt_ui.options();
	auto old = opts.align;
	opts.align = 0;

	int x = pRect->left + nX;
	int y = pRect->top;
	int w = pRect->right - pRect->left;
	int h = pRect->bottom - pRect->top;

	if (nStyle & 1)
		opts.align = AL::AL_HCENTER;
	else if (nStyle & 2)
		opts.align = AL::AL_RIGHT;
	if (nStyle & 4)
		y += (h - g_fnt_ui.lineHeight()) / 2;

	g_fnt_ui.setMixColor(ck::rgb(GetRValue(color),GetGValue(color),GetBValue(color)));
	g_fnt_ui.draw(chrs, pSur, x, y, w);
	opts.align = old;

	return 0;
}

std::vector<std::string> g_search_dirs;
int Re_BufferIOFileClass::Set_Name(const char* fileName)
{
	for (auto& it : g_search_dirs)
	{
		auto path = find_file(g_module_dir + it, fileName, false);
		if (!path.empty())
			return H_Set_Name(this, path.c_str());
	}

	return H_Set_Name(this, fileName);
}

static std::map<void*, MyFont*> g_mapFnt;
MSFont* Re_MSFont::constructor(bool bSide)
{
	if (bSide)
	{
		if (Scen->IsGDI)
			g_mapFnt[this] = &g_fullfnt3gdi;
		else
			g_mapFnt[this] = &g_fullfnt3nod;
	}
	return H_constructor(this, bSide);
}

void Re_MSFont::destructor()
{
	H_destructor(this);
	g_mapFnt.erase(this);
}

MyFont* Re_MSFont::fetch(MSFont* ptr)
{
	auto iter = g_mapFnt.find(ptr);
	if (iter == g_mapFnt.end())
		return &g_fullfnt3;
	return iter->second;
}
