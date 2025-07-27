
// exe.cpp: 定义应用程序的类行为。
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "exe.h"
#include "MainWnd.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CApp 构造
CApp::CApp() noexcept
{

	// TODO: 将以下应用程序 ID 字符串替换为唯一的 ID 字符串；建议的字符串格式
	//为 CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("TiberianSun.I18N.Version"));

	
	// TODO:  在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}

// 唯一的 CApp 对象

CApp theApp;

// CApp 初始化
BOOL CApp::InitInstance()
{
	CWinApp::InitInstance();

	EnableTaskbarInteraction(FALSE);

	get_module_dir();
	auto pFrame = new CMainWnd;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	pFrame->DoModal();
	delete pFrame;
	return TRUE;
}

int CApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}
