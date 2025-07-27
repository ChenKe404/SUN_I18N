#ifndef PATCH_H
#define PATCH_H

#include <stdint.h>

enum OPCODE {
    OP_CALL = 0xE8,     // call
    OP_JMP = 0xE9,      // jmp
};

#pragma pack(1)     // 1字节对齐
struct patch_cmd
{
    uint8_t op = 0;
    uintptr_t addr = 0;
};
#pragma pack()

constexpr auto size_cmd = sizeof(patch_cmd);

// 读取目标地址的指令
inline bool read_cmd(uintptr_t address, patch_cmd& out)
{
    SIZE_T bytes;
    ReadProcessMemory(GetCurrentProcess(), (LPVOID)address, &out, size_cmd, &bytes);
    return bytes == size_cmd;
}

// 向目标地址写入指令
template<typename T>
inline bool write_cmd(uintptr_t address, T new_address, OPCODE opcode)
{
    static_assert(size_cmd == 5, "Call struct not expected size!");

    SIZE_T bytes;
    patch_cmd cmd{
        (uint8_t)opcode,
        reinterpret_cast<uintptr_t>((void*&)new_address) - address - size_cmd
    };

    WriteProcessMemory(GetCurrentProcess(), (LPVOID)address, &cmd, size_cmd, &bytes);
    return bytes == size_cmd;
}

#endif // !PATCH_H
