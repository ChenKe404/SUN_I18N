#include <set>
#include <fstream>
#include <common/patch.h>
#include <bio/bio.hpp>
#include <TSpp/src/tibsun/tibsun_globals.h>
#include "res/resource.h"

HWND g_MainWnd;

struct resol
{
    int width, height;
    inline bool operator<(const resol& o) const
    {
        if (width < o.width)
            return true;
        if (width > o.width)
            return false;
        if (height < o.height)
            return true;
        return false;
    }
};
static std::vector<int> g_resolution;
static const int* enum_resolution()
{
	g_resolution.clear();
	// 获取主显示器设备名称
	DISPLAY_DEVICE displayDevice = { sizeof(DISPLAY_DEVICE) };
	DWORD idx = 0;
	LPCTSTR primaryDevice = nullptr;

	while (EnumDisplayDevices(nullptr, idx, &displayDevice, 0)) {
		if (displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
			primaryDevice = displayDevice.DeviceName;
			break;
		}
		idx++;
	}

	if(primaryDevice)
	{
		// 枚举所有显示模式
		DEVMODE devMode = {};
		devMode.dmSize = sizeof(DEVMODE);
		std::set<resol> resols;

		for (int modeIndex = 0; ; modeIndex++) {
			if (!EnumDisplaySettings(primaryDevice, modeIndex, &devMode))
				break;
			// 过滤无效模式
			if ((devMode.dmPelsWidth < 640 || devMode.dmPelsHeight < 480) || devMode.dmBitsPerPel < 16)
				continue;

			resols.insert({ (int)devMode.dmPelsWidth, (int)devMode.dmPelsHeight });
		}

		g_resolution.reserve(resols.size()+1);
		for (auto& it : resols)
		{
			g_resolution.push_back(it.width);
			g_resolution.push_back(it.height);
		}
		g_resolution.push_back(0);
		g_resolution.push_back(0);
	}
	if (g_resolution.empty())
	{
		g_resolution = {
			640,480,
			800,600,
			0,0
		};
	}

	// 按分辨率排序
	return g_resolution.data();
}

__declspec(noinline) __declspec(naked)
void patch_resolution()
{
	__asm {
        mov eax, 0x007E4745
		mov [eax],1
		call enum_resolution
		mov ecx, 0x0050B527
		jmp ecx
	}
}

static void __fastcall patch_unit_select1(
	Surface* pSur, ConvertClass* a2,
	int16_t* a3, unsigned int a4,
	Point2D* xy, const Rect* a6,
	int a7, int a8, int a9, int a10,
	int a11, int a12, unsigned int a13,
	Point2D* a14, int a15
)
{}

static void* __fastcall patch_unit_select(const char* fileName)
{
	static const auto Fx = (void*(*)(const char*))0x00559DE0;
	auto hrc = FindResourceA(hInstance, MAKEINTRESOURCEA(IDR_SHP_UNIT_SELECT), "SHP");
	if (!hrc) return Fx(fileName);
	auto hg = LoadResource(hInstance, hrc);
	if (!hg)
	{
		DeleteObject(hrc);
		return Fx(fileName);
	}

	return LockResource(hg);
}

void init_patch_misc()
{
	write_cmd(0x0050B4F7, patch_resolution, OP_JMP);
	if (g_setting.hide_unit_select)
		write_cmd(0x00587AE5, patch_unit_select, OP_CALL);
}