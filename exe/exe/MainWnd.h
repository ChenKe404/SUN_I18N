
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include <map>

class CMainWnd : public CDialogEx
{
	enum { IDD = IDD_MAINWND };
public:
	CMainWnd() noexcept;
	void DoDataExchange(CDataExchange* pDX) override;
	BOOL OnInitDialog() override;
	void OnOK() override;
	void OnCancel() override;
protected:
	void loadLang(const char* loc);
	void updateSetting(bool save);
protected: 
	DECLARE_DYNAMIC(CMainWnd)

	CStatic _stc_img;
	CComboBox _cob_lang;

	CImage _img;
	RECT _rc_img;

	BOOL _chk_windowed;
	BOOL _chk_hide_unit_select;
	BOOL _chk_movie_subtitle;
	std::map<HWND, std::string> _ctl_text;

// 生成的消息映射函数
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDrawItem(int nID, LPDRAWITEMSTRUCT lpDrst);
	afx_msg void OnLangChanged();
};


