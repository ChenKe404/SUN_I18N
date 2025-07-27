#include "resource.h"

MyText g_text;

ck::Font g_ck_fnt12r;
ck::Font g_ck_fnt12ro;
ck::Font g_ck_fnt16r;
ck::Font g_ck_fnt16ro;

MyFont g_fnt12;
MyFont g_fnt12o;
MyFont g_fnt_subtitle;
MyFont g_fnt_ui;
MyFont g_fullfnt3;
MyFont g_fullfnt3gdi;
MyFont g_fullfnt3nod;

bool init_resource()
{
    const auto loc_dir = g_module_dir + "i18n/" + g_setting.loc + "/";
    // 12点 常规
    std::string path = loc_dir +"fnt12r.ckf";
    if (!g_ck_fnt12r.load(path))
    {
        error("cannot found font: " + path, "Font Lost");
        return false;
    }

    // 12点 常规 + 轮廓
    path = loc_dir + "fnt12ro.ckf";
    if (!g_ck_fnt12ro.load(path))
    {
        error("cannot found font: " + path, "Font Lost");
        return false;
    }

    // 16点 常规
    path = loc_dir + "fnt16r.ckf";
    if (!g_ck_fnt16r.load(path))
    {
        error("cannot found font: " + path,"Font Lost");
        return false;
    }

    // 16点 常规 + 轮廓
    path = loc_dir + "fnt16ro.ckf";
    if (!g_ck_fnt16ro.load(path))
    {
        error("cannot found font: " + path, "Font Lost");
        return false;
    }

    g_fnt12.attach(&g_ck_fnt12r);
    auto opts = &g_fnt12.options();
    opts->spacingY = 1;
    opts->align = 0;
    opts->breakWord = true;

    g_fnt12o.attach(&g_ck_fnt12ro);
    g_fnt12o.options() = *opts;

    // 字幕字体
    g_fnt_subtitle.attach(&g_ck_fnt16ro);
    opts = &g_fnt_subtitle.options();
    opts->spacingY = 2;
    opts->breakWord = true;

    g_fnt_ui.attach(&g_ck_fnt12r);
    opts = &g_fnt_ui.options();
    opts->breakWord = true;

    g_fullfnt3.attach(&g_ck_fnt16r);
    opts = &g_fullfnt3.options();
    opts->spacingY = 2;
    opts->align = 0;
    opts->breakWord = true;

    g_fullfnt3gdi = g_fullfnt3;
    g_fullfnt3gdi.setMixColor(ck::rgb(32, 214, 97));
    g_fullfnt3nod = g_fullfnt3;
    g_fullfnt3nod.setMixColor(ck::rgb(255, 28, 28));

    path = loc_dir + "local.ckt";
    if (!g_text.open(path.c_str()))
    {
        error("cannot found local: " + path, "Local Lost");
        return false;
    }

    g_search_dirs.push_back("i18n/" + g_setting.loc + "/asserts");

    return true;
}
