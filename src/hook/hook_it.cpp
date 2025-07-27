#include "hook_it.h"
#include <assert.h>
#include <stdint.h>

#include <windows.h>

using namespace ck;

#pragma pack(1)
struct CMD {
    uint8_t op;
    void* adr;
};
#pragma pack()

inline static void jump(void* sourceFunc, void* targetFunc)
{
    //用于保存原来的内存访问flag
    DWORD oldpro = 0;

    //改变原函数内存地址前5个字节的访问权限为可读可写
    VirtualProtect(sourceFunc, 5, PAGE_EXECUTE_READWRITE, &oldpro);

    /*
    * 0xe9是汇编指令jmp的16进制码,占一个字节
    * jmp 0x10000000 -> 0xe910000000
    * jmp指令 + 4字节函数地址 = 5字节空间
    */
    *(BYTE*)sourceFunc = 0xe9;

    *(INT_PTR*)((BYTE*)sourceFunc + 1) = (INT_PTR)targetFunc - (INT_PTR)sourceFunc - 5;
}

inline static void write_cmd(void* adr,const uint8_t cmd[5])
{
    DWORD oldpro = 0, pro = 0;
    VirtualProtect(adr, 5, PAGE_EXECUTE_READWRITE, &oldpro);
    memcpy(adr, cmd, 5);
    VirtualProtect(adr, 5, oldpro, &pro);  // 还原保护
}

class HookItImp : public ck::HookIt
{
public:
    ~HookItImp()
    {
        HookItImp::unhook(); 
    }

    HookItImp(const char* dll_name, const char* old_func_name, void* new_func_ptr)
    {
        assert(dll_name && old_func_name);
        _new_ptr = new_func_ptr;
        HMODULE module = GetModuleHandleA(dll_name);
        assert(module != NULL);
        _old_ptr = (void*)GetProcAddress(module, old_func_name);

        memcpy(_cmd_old, _old_ptr, 5);
    }

    HookItImp(void* com_obj_ptr, uint32_t com_func_num, void* new_func_ptr)
    {
        assert(com_obj_ptr && new_func_ptr);
        _new_ptr = new_func_ptr;
        _old_ptr = (void*)*(INT_PTR*)(*(INT_PTR*)com_obj_ptr + (com_func_num * 4 - 4));

        memcpy(_cmd_old, _old_ptr, 5);
    }

    HookItImp(void* old_func_ptr, void* new_func_ptr)
    {
        assert(old_func_ptr && new_func_ptr);
        _new_ptr = new_func_ptr;
        _old_ptr = old_func_ptr;

        memcpy(_cmd_old, _old_ptr, 5);
    }

    const bool hooked() const override
    {
        return _hooked;
    }

    void unhook() override
    {
        _hooked = false;
        write_cmd(_old_ptr, _cmd_old);
    }

    void hook() override
    {
        _hooked = true;
        jump(_old_ptr, _new_ptr);
    }
private:
    bool _hooked = false;
    uint8_t _cmd_old[5];	//保存原始地址的指令
    void* _old_ptr = NULL;	//原函数指针
    void* _new_ptr;		    //目标函数地址
};


HookItPtr ck::HookIt::$(const char* dll, const char* srcFunc, void* desFunc)
{
    return HookItPtr(new HookItImp(dll, srcFunc, desFunc));
}

HookItPtr ck::HookIt::$(void* obj, uint32_t comFuncNum, void* desFunc)
{
    return HookItPtr(new HookItImp(obj, comFuncNum, desFunc));
}

HookItPtr ck::HookIt::$(void* srcFunc, void* desFunc)
{
    return HookItPtr(new HookItImp(srcFunc, desFunc));
}
