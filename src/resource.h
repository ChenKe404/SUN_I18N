#ifndef SUN_DLL_GLOBAL_H
#define SUN_DLL_GLOBAL_H

#include <map>
#include <string>
#include <font.h>
#include "my_font.h"
#include <cktext/text.h>

struct MyText : protected ck::Text
{
	using super = ck::Text;
	inline bool open(const char* path) {
		return super::open(path);
	}
	inline u8str u8(u8str src) const {
		auto trs = super::u8(src, src);
		if (!trs) trs = src;
		return trs;
	}
	inline u32str u32(u8str src) const {
		static std::u32string u32str;
		auto trs = super::u32(src, src);
		if (!trs) {
			u8to32(src, u32str);
			trs = u32str.c_str();
		}
		return trs;
	}
};


extern MyText g_text;

extern MyFont g_fnt12;
extern MyFont g_fnt12o;
extern MyFont g_fnt_subtitle;
extern MyFont g_fnt_ui;
extern MyFont g_fullfnt3;
extern MyFont g_fullfnt3gdi;
extern MyFont g_fullfnt3nod;

constexpr auto to_u32str = ck::Text::u8to32;

inline std::string to_u8local(const char* anstr)
{
    std::string ret;
    if (!anstr) return ret;
	return  g_text.u8(anstr);
}

inline std::u32string to_u32local(const char* anstr)
{
    std::u32string ret; 
    if (!anstr) return ret;
    auto str = g_text.u32(anstr);
    if (str) ret = str;
    else ck::Text::u8to32(anstr, ret);   // 没有译文则直接使用原文
    return ret;
}

bool init_resource();

#endif // !SUN_DLL_GLOBAL_H
