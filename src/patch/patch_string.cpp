#include <technotype.h>

/*
* ��Ϸ��ini���ؿƼ�����ʱ���Ƽ�����ת��Ϊutf8���������
*/

static void __stdcall local_memcpy(char* dst, const char* src, size_t count)
{
	auto loc = to_u8local(src);
	auto len = std::min(loc.size(), count);
	if (len > 0)
	{
		// NOTE: Ӣ�����֧��49���ַ�, ���һ���ַ���������\0, ��������utf8��������ɵ��ַ����ܸ���, ����������Ҫ����
		memset(dst, 0, count);
		memcpy(dst, loc.c_str(), len);
	}
}

void __stdcall local_techno_name(AbstractTypeClass* that)
{
	local_memcpy(that->FullName, that->FullName, 48);
	that->FullName[48] = 0;
}

// ��Rules.ini��ȡ�Ƽ�����֮��, ����ȡ���ĿƼ�����ת��Ϊ��������
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

// ����籩��ԭ�����в�ͬ
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
	write_cmd(0x0063D9B1, patch_techno_load, OP_JMP);	// ԭ��
	write_cmd(0x00406406, patch_techno_load_ini, OP_JMP);	// ����籩
}