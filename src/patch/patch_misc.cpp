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

static const int* __fastcall enum_resolution(int, int, int, int, int)
{
	g_HighResolution = true;

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

	std::vector<int> resolution;
	if (primaryDevice)
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

		resolution.reserve(resols.size() + 1);
		for (auto& it : resols)
		{
			resolution.push_back(it.width);
			resolution.push_back(it.height);
		}
		resolution.push_back(0);
		resolution.push_back(0);
	}
	if (resolution.empty())
	{
		resolution = {
			640,480,
			800,600,
			0,0
		};
	}

	static auto _nh_malloc = (int(__cdecl*)(size_t, int))0x006B72DE;
	const auto count = resolution.size() * sizeof(int);
	auto mem = (int*)_nh_malloc(count, 1);
	memcpy(mem, resolution.data(), count);
	return mem;
}

static void* __fastcall patch_unit_select(const char* fileName)
{
	static const auto Fx = (void* (*)(const char*))0x00559DE0;
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
	write_cmd(0x0050B522, enum_resolution, OP_CALL);
	if (g_setting.hide_unit_select)
		write_cmd(0x00587AE5, patch_unit_select, OP_CALL);
}