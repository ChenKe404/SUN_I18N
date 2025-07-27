#include "pch.h"
#include "inject.h"
#include <windows.h>
#include <iostream>

#define Out std::cout
#define Err std::cerr
#define End std::endl

bool doInject(const std::string& dll_path, const std::string& exe_path)
{
    LPSTR dllPath = (LPSTR)dll_path.c_str();
    size_t dllPathLen = dll_path.size();

    LPSTR exePath = (LPSTR)exe_path.c_str();
    size_t exePathLen = exe_path.size();

    size_t sepIndex = exe_path.find_last_of("/\\");
    std::string workDir = exe_path.substr(0, sepIndex + 1);
    if (sepIndex == MAXSIZE_T)
        workDir = "./";

    Out << "工作目录: " << workDir << End;

    //启动目标进程
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;

    PROCESS_INFORMATION pi;
    BOOL ret = ::CreateProcessA(NULL, exePath, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, workDir.c_str(), &si, &pi);
    //获得当前进程的句柄
    HANDLE processHandle = pi.hProcess;
    if (!ret || processHandle == NULL) {
        Err << "创建进程失败!" << End;
        return false;
    }

    //创建虚拟内存地址,放置dll路径
    LPVOID injectAddress = VirtualAllocEx(processHandle, NULL, dllPathLen + 1, MEM_COMMIT, PAGE_READWRITE);
    if (injectAddress == NULL){
        Err << "开辟虚拟内存空间失败,内存空间不足或权限不够!" << End;
        CloseHandle(processHandle);
        ResumeThread(pi.hThread);
        return false;
    }
    ret = WriteProcessMemory(processHandle, injectAddress, dllPath, dllPathLen + 1, NULL);
    if (!ret) {
        Err << "写入数据到进程内存失败!" << End;
        VirtualFreeEx(processHandle, dllPath, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        ResumeThread(pi.hThread);
        return false;
    }
    // 获取LoadLibraryA模块:用于加载dll;
    HMODULE module = GetModuleHandleA("kernel32");
    if (module == NULL) {
        Err << "kernel32模块加载失败!" << End;
        VirtualFreeEx(processHandle, dllPath, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        ResumeThread(pi.hThread);
        return false;
    }

    PTHREAD_START_ROUTINE pfnLoadLib = (PTHREAD_START_ROUTINE)GetProcAddress(module, "LoadLibraryA");
    //在线程中执行dll中的入口函数：即导入dll
    HANDLE remoteThread = CreateRemoteThread(processHandle, NULL, 0, pfnLoadLib, injectAddress, 0, NULL);
    if (remoteThread == NULL) {
        Err << "创建远程线程失败!" << End;
        VirtualFreeEx(processHandle, dllPath, 0, MEM_RELEASE);
        CloseHandle(processHandle);
        ResumeThread(pi.hThread);
        return false;
    }

    WaitForSingleObject(remoteThread, INFINITE);
    VirtualFreeEx(processHandle, dllPath, 0, MEM_RELEASE);
    CloseHandle(remoteThread);
    CloseHandle(processHandle);
    //恢复进程运行
    ResumeThread(pi.hThread);
    return true;
}
