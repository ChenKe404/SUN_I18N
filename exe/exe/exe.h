
// exe.h: exe 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含 'pch.h' 以生成 PCH"
#endif

#include "resource.h"       // 主符号

class CApp : public CWinApp
{
public:
	CApp() noexcept;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
};

extern CApp theApp;
