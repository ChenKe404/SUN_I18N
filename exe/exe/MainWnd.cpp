
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "exe.h"
#include "inject.h"
#include <algorithm>
#define MINI_CASE_SENSITIVE
#include <mini/ini.h>
using namespace mINI;

#include "MainWnd.h"

// CMainFrame
IMPLEMENT_DYNAMIC(CMainWnd, CDialogEx)

// CMainFrame 构造/析构
CMainWnd::CMainWnd() noexcept
	: CDialogEx(IDD)
{}

void CMainWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STC_PICTURE, _stc_img);
	DDX_Control(pDX, IDC_COB_LANG, _cob_lang);
	DDX_Check(pDX, IDC_CHK_WINDOWED, _chk_windowed);
	DDX_Check(pDX, IDC_CHK_UNIT_LINE, _chk_hide_unit_select);
	DDX_Check(pDX, IDC_CHK_SUBTITLE, _chk_movie_subtitle);
}

std::string make_local_desc(LCID lcid)
{
	char buf[256];
	std::string desc;
	auto ret = GetLocaleInfoA(lcid, LOCALE_SNATIVELANGNAME, buf, 256);
	if (ret > 0) desc.append(buf);
	ret = GetLocaleInfoA(lcid, LOCALE_SNATIVECTRYNAME, buf, 256);
	if (ret > 0) desc.append(" - ").append(buf);
	return desc;
}

BOOL CMainWnd::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	auto hInstance = GetModuleHandleA(0);

	// 加载图片, 计算图片绘制的宽高和位置
	auto hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 0, 0, 0);
	SetIcon(hIcon, false);

	RECT rc;
	_stc_img.GetClientRect(&rc);
	_img.LoadFromResource(hInstance, IDB_BITMAP);
	int w = _img.GetWidth(), h = _img.GetHeight();
	float ratio = w / (float)h;
	auto cw = rc.right - rc.left;
	auto ch = rc.bottom - rc.top;
	auto rw = cw / (float)w;
	auto rh = ch / (float)h;
	int ox = 0, oy = 0;
	if (rw < rh)
	{
		w = cw;
		h = w / ratio;
	}
	else
	{
		h = ch;
		w = h * ratio;
	}
	_rc_img = {
		(cw - w) / 2,
		(ch - h) / 2,
		w,h
	};
	_rc_img.right += _rc_img.left;
	_rc_img.bottom += _rc_img.top;

	// 记录控件的初始文本
	char text[256];
	auto hChild = ::GetWindow(m_hWnd, GW_CHILD);
	GetWindowTextA(m_hWnd, text, 256);
	_ctl_text[m_hWnd] = text;
	while (hChild)
	{
		if (hChild != _cob_lang)
		{
			GetWindowTextA(hChild, text, 256);
			_ctl_text[hChild] = text;
		}
		hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
	}

	auto path = g_module_dir + "SUN_I18N.ini";


	// 读取国际化目录, 填充下拉框并加载默认语言
	char buf[256];
	auto lcid = GetUserDefaultLCID();
	GetLocaleInfoA(lcid, LOCALE_SNAME, buf, 256);
	std::string sys_loc(buf);

	int idx = 0;
	int sel_idx = 0;
	std::wstring wstr;

	// 初始英文
	lcid = LocaleNameToLCID(L"en-US", 0);
	if (lcid == 0)
		idx = _cob_lang.AddString(L"en-US");
	else
	{
		auto desc = make_local_desc(lcid);
		if (desc.empty()) desc = "en-US";
		ansi_to_wide(desc.c_str(), wstr);
		idx = _cob_lang.AddString(wstr.c_str());
	}
	if (equalnc("en-US", sys_loc.c_str()))
		sel_idx = idx;
	_cob_lang.SetItemData(idx, 0);

	// 搜索国际化目录
	auto dir = g_module_dir + "i18n";
	auto subs = enum_children(dir, 1);
	for (auto& it : subs)
	{
		auto path = dir + "/" + it + "/window.ini";
		if (!PathFileExistsA(path.c_str()))
			continue;
		ansi_to_wide(it.c_str(), wstr);
		auto lcid = LocaleNameToLCID(wstr.c_str(),0);
		if (lcid == 0)
			idx = _cob_lang.AddString(wstr.c_str());
		else
		{
			auto desc = make_local_desc(lcid);
			if (desc.empty()) desc = it;
			ansi_to_wide(desc.c_str(), wstr);
			idx = _cob_lang.AddString(wstr.c_str());
		}
		if (equalnc(it.c_str(), sys_loc.c_str()))
			sel_idx = idx;

		const auto len = it.size();
		auto data = new char[len + 1];
		memcpy(data, it.data(), len);
		data[len] = '\0';
		_cob_lang.SetItemData(idx, (DWORD_PTR)data);
	}

	_cob_lang.SetCurSel(sel_idx);

	updateSetting(false);
	OnLangChanged();

	return 0;
}

void CMainWnd::OnOK()
{
	updateSetting(true);
	std::string exe("Game.exe");
	if (_chk_windowed)
		exe.append(" -win");
	doInject("SUN.dll", exe);
	CDialogEx::OnOK();
}

void CMainWnd::OnCancel()
{
	updateSetting(true);
	CDialogEx::OnCancel();
}

void CMainWnd::loadLang(const char* loc)
{
	// 还原英文
	if (!loc || equalnc(loc, "en-us"))
	{
		auto iter = _ctl_text.find(m_hWnd);
		if (iter != _ctl_text.end())
			SetWindowTextA(m_hWnd, iter->second.c_str());
		auto hChild = ::GetWindow(m_hWnd, GW_CHILD);
		while (hChild)
		{
			iter = _ctl_text.find(hChild);
			if (iter != _ctl_text.end())
				SetWindowTextA(hChild, iter->second.c_str());
			hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
		}
		return;
	}

	auto path = g_module_dir + "i18n/" + loc + "/window.ini";
	INIFile fi(path);
	INIStructure ini;
	if (!fi.read(ini))
		return;

	char text[256];
	GetWindowTextA(m_hWnd, text, 256);
	auto& trs = ini[text]["Text"];
	if (!trs.empty())
		SetWindowTextA(m_hWnd, trs.c_str());

	auto hChild = ::GetWindow(m_hWnd, GW_CHILD);
	while (hChild)
	{
		if (hChild != _cob_lang)
		{
			GetWindowTextA(hChild, text, 256);
			auto& trs = ini[text]["Text"];
			if (!trs.empty())
				SetWindowTextA(hChild, trs.c_str());
		}
		hChild = ::GetNextWindow(hChild, GW_HWNDNEXT);
	}
}

void CMainWnd::updateSetting(bool save)
{
	const auto path = g_module_dir + "SUN_I18N.ini";
	INIFile fi(path);
	INIStructure ini;
	if (save)
	{
		UpdateData();
		auto idx = _cob_lang.GetCurSel();
		auto loc = (const char*)_cob_lang.GetItemData(idx);
		if (!loc) loc = "en-us";

		auto& sec = ini["Setting"];
		sec["Lang"] = loc;
		sec["Windowed"] = _chk_windowed ? "Yes" : "No";
		sec["HideUnitSelect"] = _chk_hide_unit_select ? "Yes" : "No";
		sec["MovieSubtitle"] = _chk_movie_subtitle ? "Yes" : "No";
		fi.write(ini, true);
	}
	else
	{
		fi.read(ini);
		auto& sec = ini["Setting"];
		const auto& loc = sec["Lang"];
		_chk_windowed = equalnc(sec.get("Windowed").c_str(),"Yes");
		_chk_hide_unit_select = equalnc(sec.get("HideUnitSelect").c_str(),"Yes");
		_chk_movie_subtitle = equalnc(sec.get("MovieSubtitle").c_str(),"Yes");
		if (!loc.empty())
		{
			if (equalnc(loc.c_str(), "en-us"))
				_cob_lang.SetCurSel(0);
			else
			{
				const auto count = _cob_lang.GetCount();
				for (int i = 0;i < count;++i)
				{
					auto _loc = (const char*)_cob_lang.GetItemData(i);
					if (_loc && equalnc(loc.c_str(), _loc))
					{
						_cob_lang.SetCurSel(i);
						break;
					}
				}
			}
		}
		UpdateData(false);
	}
}

BEGIN_MESSAGE_MAP(CMainWnd, CDialogEx)
	ON_WM_DRAWITEM()
	ON_CBN_SELCHANGE(IDC_COB_LANG, &CMainWnd::OnLangChanged)
END_MESSAGE_MAP()

void CMainWnd::OnDrawItem(int nID, LPDRAWITEMSTRUCT lpDrst)
{
	if (nID == IDC_STC_PICTURE)
	{
		_img.Draw(lpDrst->hDC,
			_rc_img,
			Gdiplus::InterpolationModeHighQuality
		);
	}
}

void CMainWnd::OnLangChanged()
{
	auto cur = _cob_lang.GetCurSel();
	auto loc = _cob_lang.GetItemData(cur);
	loadLang((const char*)loc);
}
