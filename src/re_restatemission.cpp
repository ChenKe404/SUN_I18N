#include "re_restatemission.h"

#include <tibsun/tibsun_functions.h>
#include <scenario.h>
#include "tspp_ex/definitions.h"
#include "my_wordanim.h"
#include "dohook.h"

MyFont::CharPtrs g_chrs;
std::list<MyFont::Lines> g_pages;
const MyFont::Lines* g_curPage;

const char* RE_RestateMission::Draw_Text(Surface* pSur)
{
    auto& the = *(RestateMission*)this;
    MyFont& fnt = Scen->IsGDI ? g_fullfnt3gdi : g_fullfnt3nod;
    if (g_curPage && !g_chrs.empty())
    {
        auto& rc = the.mRect;
        for (auto& line : *g_curPage)
        {
            fnt.draw(g_chrs, line, pSur, rc.X, rc.Y);
        }
    }
    if (the.mBtnResume)
        the.mBtnResume->Draw_All(true);
    return "";
}

bool RE_RestateMission::Start_Briefing_Screen(ScenarioClass* pSce)
{
    auto& the = *(RestateMission*)this;
    MyFont& fnt = Scen->IsGDI ? g_fullfnt3gdi : g_fullfnt3nod;
    bool ret = false;
    if (!pSce || !g_SurAlternate || !g_SurHidden)
        return 0;
    if (the.Read_Mission_Ini(pSce))
    {
        g_Keyboard->Clear();
        g_Mouse->Release_Mouse();
        g_Mouse->Hide_Mouse();

        Load_Title_Screen(g_aPcxScore, g_SurAlternate, &g_PalOriginal);
        g_SurHidden->Copy_From(*g_SurAlternate);
        the.Invalidate(g_SurHidden->Get_Rect());
        the.Load_Cursor(g_SurHidden);
        auto& mRect = the.mRect;
        auto& vBtns = the.vBtns;
        auto& aText = the.mText;

        if (strlen(aText) > 0)
        {
            Rect rect = { the.nX + 110, the.nY + 60, 420, 280 };
            auto u32str = (char32_t*)g_text.u32(aText);
            prepare_text(u32str);
            g_chrs = fnt.cs(u32str);
            g_pages.clear();
            {
                MyFont::Lines lines_total;
                mRect = fnt.measure(g_chrs, 420, -1, &lines_total);
                mRect.X = rect.X + (rect.Width - mRect.Width) / 2;
                mRect.Y = rect.Y + (rect.Height - mRect.Height) / 2;
                mRect = Intersect(rect, mRect);     // 计算背景与文本的相交区域并重新赋值回mRect -> lib/rect.h

                const auto spc_y = fnt.options().spacingY;
                const auto lineHeight = fnt.lineHeight();
                int lines_per_page = rect.Height / (lineHeight + spc_y);    // 每页可容纳的行数
                if (lines_per_page >= lines_total.size())
                    g_pages.push_back(lines_total);
                else
                {
                    MyFont::Lines lines_temp;
                    int lines_remain = lines_total.size();
                    int pos = 0;
                    while (lines_remain > 0)
                    {
                        auto iter = lines_total.begin() + pos;
                        if(lines_remain >= lines_per_page)
                            lines_temp.assign(iter, iter + lines_per_page);
                        else
                            lines_temp.assign(iter, lines_total.end());
                        if (!g_pages.empty()) // 换页重置垂直偏移
                        {
                            const auto oy = lines_total.front().oy - lines_temp.front().oy;
                            for (auto& line : lines_temp)
                            {
                                line.oy += oy;
                            }
                        }

                        g_pages.push_back(lines_temp);
                        lines_remain -= lines_per_page;
                        pos += lines_per_page;
                    }
                }
            }

            g_curPage = nullptr;
            int num = (int)g_pages.size();
            for (auto& lines : g_pages)
            {
                g_curPage = &lines;
                auto anim = new MYWordAnim(&fnt, &g_chrs, &lines, mRect);
                the.Attach_Anim(anim);
                the.Play_Anim(anim, 300000);
                if (num < 2)  // 控制是否显示"更多"按钮
                    break;
                num--;

                g_Mouse->Show_Mouse();

                /* 翻页按钮 */
                const auto count = vBtns.Count();
                for (int i = 0;i < count;++i)
                {
                    auto pBtn = vBtns[i];
                    if (!pBtn || pBtn->ID != 3)     // 翻页按钮
                        continue;
                    pBtn->X = mRect.X + mRect.Width / 2 - pBtn->Width / 2;
                    pBtn->Y = mRect.Y + mRect.Height;
                    pBtn->Enable();
                    the.Wait_Resume();
                    pBtn->Redraw();
                    pBtn->Disable();
                    break;
                }
 
                g_Mouse->Hide_Mouse();
                g_SurHidden->Copy_From(rect, *g_SurAlternate, rect, 0, 1);
                the.Invalidate(rect);
            }
        }

        /* 启用按钮 */
		const auto count = vBtns.Count();
		for (int i = 0;i < count;++i)
		{
            auto pBtn = vBtns[i];
            if (!pBtn) continue;
            if(pBtn->ID == 1)   // Resume按钮
                pBtn->Enable();
            if(pSce->BriefMovie != VQ_NONE && pBtn->ID == 2)   // Video按钮
                pBtn->Enable();
		}

        /* 等待按钮和恢复键盘鼠标 */
        g_Mouse->Show_Mouse();
        ret = the.Wait_Resume();
        g_Mouse->Hide_Mouse();
        g_SurHidden->Fill(0);
        the.Invalidate(g_SurHidden->Get_Rect());
        the.Load_Cursor(g_SurHidden);
        the.Reset();
        g_Keyboard->Clear();
        g_Mouse->Show_Mouse();
        g_Mouse->Capture_Mouse();
    }
    return ret;
}
