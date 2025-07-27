#include <technotype.h>

/*
* 游戏从ini加载科技配置时将科技名称转换为utf8编码的译文
*/

static void __stdcall local_memcpy(char* dst, const char* src, size_t count)
{
	auto loc = to_u8local(src);
	auto len = std::min(loc.size(), count);
	if (len > 0)
	{
		// NOTE: 英文最多支持49个字符, 最后一个字符还必须是\0, 本地名称utf8编码可容纳的字符可能更少, 后续可能需要扩容
		memset(dst, 0, count);
		memcpy(dst, loc.c_str(), len);
	}
}

void __stdcall local_techno_name(AbstractTypeClass* that)
{
	local_memcpy(that->FullName, that->FullName, 48);
	that->FullName[48] = 0;
}

// 从Rules.ini读取科技配置之后, 将读取到的科技名称转换为本地名称
__declspec(noinline) __declspec(naked)
void patch_techno_load()
{
	int vret;
	__asm {
		mov ecx, 0x00588DD0
		call ecx
		mov vret, eax
		push edi
		call local_techno_name
		mov eax, vret
		mov ecx, 0x0063D9B6
		jmp ecx
	}
}

// 火焰风暴与原版略有不同
__declspec(noinline) __declspec(naked)
void patch_techno_load_ini()
{
	int vret;
	__asm {
		push 48
		lea ecx, [esp+0x14]
		push ecx
		push edi
		call local_memcpy
		mov ecx, 0x00406412
		jmp ecx
	}
}

void init_patch_string()
{
	write_cmd(0x0063D9B1, patch_techno_load, OP_JMP);	// 原版
	write_cmd(0x00406406, patch_techno_load_ini, OP_JMP);	// 火焰风暴
}