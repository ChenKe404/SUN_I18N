#include "re_msglist.h"
#include <ui/txtlabel.h>
#include <msglist.h>
#include <voc.h>
#include "my_wwfont.h"
#include "resource.h"
#include <tspp_ex/definitions.h>

int32_t& nInterval_74C610 = Make_Global<int32_t>(0x0074C610);
int32_t& nTime_74C608 = Make_Global<int32_t>(0x0074C608);
void*& dword_74C488 = Make_Global<void*>(0x0074C488);

int Re_MessageList::Init(
    int a2, int a3, int a4, int a5, 
    int a6, int a7, int a8, bool a9, 
    int a10, int a11, int a12
) {
    a6 = g_fnt12o.lineHeight() + 2;
    if (a6 < 1) a6 = 14;
    else a6 += 2;   // 多两个像素间隔
    return H_Init(this, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12);
}

TextLabelClass* Re_MessageList::Add_Message(const char* a2, int a3, char* message, ColorSchemeType cst, TextPrintType style, int a7) 
{
    if (!message)
        return 0;
    auto trs = g_text.u32(message);
    // 没有译文则使用原始函数
    if (!trs)
        return H_Add_Message(this, a2, a3, message, cst, style, a7);
    // 字体
    auto wwfnt = wwfont_ptr(style);
    if (!wwfnt)
        return 0;
    auto fnt = wwfnt->get();
    if (!fnt)
        return 0;
    // 构造完整u32字符串
    std::u32string u32;
    if (a2)  // %s:  -> xxxx:
    {
        ck::Text::u8to32(u32, a2);
        u32.push_back(':');
    }
    u32.append(trs);

    const auto chrs = fnt->cs(u32);
    // 字符串长度和字符指针长度不一致则需要重建原始字符串,
    // 因为后续需要按索引范围裁剪原始字符串
    if (chrs.size() != u32.size()) {
        auto len = chrs.size();
        u32.resize(len);
        for (int i = 0;i < len;++i) {
            u32[i] = chrs[i]->code;
        }
    }

    MyFont::Lines lines;
    fnt->measure(chrs, this->Width, -1, &lines);
    // 检查每一行的字符串是否会超出缓冲区容量, 将超出部分重新创建为一个行
    std::string tmp;
    for (auto i = lines.begin(); i != lines.end();)
    {
        auto ptr = u32.data() + i->left;
        ck::Text::u32to8(tmp, ptr, i->right - i->left);
        if (tmp.size() < 162) {
            ++i;
            continue;
        }
        auto right = i->right - 1;
        for (; right > i->left; --right) {
            ck::Text::u32to8(tmp, ptr, right - i->left);
            if (tmp.size() < 162)
                break;
        }
        MyFont::Line line(*i);
        line.left = right;
        line.right - i->right;
        i->right = right;
        line.width = fnt->measure(chrs.begin() + line.left, chrs.begin() + line.right).Width;
        line.ox = i->width - line.width + i->ox;
        i = lines.insert(i, line);
    }

    TextLabelClass* ret = nullptr;
    for (auto& line : lines)
    {
        if (MessageList && MaxMessages > 0)
        {
            int i = 0;
            auto list = MessageList;
            while (list) {
                list = (TextLabelClass*)list->Get_Next();
                ++i;
            }
            if (IsEdit && AdjustEdit)
                ++i;
            if (i + 1 > MaxMessages)    // 如果加入当前消息后个数超出最大消息容量, 则将第一个消息删除
            {
                auto cur = MessageList;
                MessageList = (TextLabelClass*)cur->Remove();  // 删除当前元素并返回头元素
                for (int j = 0; j < MAX_NUM_MESSAGES; ++j)
                {
                    // 将被删除的消息缓冲设为空闲
                    if (cur->Text == MessageBuffers[j]) {
                        BufferAvail[j] = 1; // 0:buffer使用中; 1:buffer空闲中
                        break;
                    }
                }
                delete cur; // 删除消息对象
            }
        }
        // 判断消息列表是否已满, 如果已满则直接返回不处理
        int idx = 0;    // 可以使用的缓冲区索引
        for (idx = 0; idx < MAX_NUM_MESSAGES && !BufferAvail[idx];)
        {
            if (++idx >= MAX_NUM_MESSAGES)
                return 0;
        }

        auto label = new TextLabelClass("", MessageX + line.ox, MessageY, cst, style);
        if (a7 == -1)
            label->UserData1 = 0;
        else
        {
            auto interval = nInterval_74C610;
            if (nTime_74C608 != -1)
                interval += GetTime() - nTime_74C608;
            label->UserData1 = a7 + interval;
        }
        label->UserData2 = a3;
        // 写入消息文本
        BufferAvail[idx] = 0;
        auto buffer = MessageBuffers[idx];
        memset(buffer, 0, 162u);
        auto ptr = u32.data() + line.left;
        ck::Text::u32to8(tmp, ptr, line.right - line.left);
        strncpy(buffer, tmp.data(), 161);   // 以防万一拷贝越界
        label->Text = buffer;
        // 音效
        __asm {
            push 0x3F800000
            mov eax, 0x0074C488
            mov eax, [eax]
            mov ecx, [eax + 0x3DC]
            mov eax, 0x00664BA0
            call eax
        }
        // 入表
        if (MessageList)
            label->Add_Tail(*MessageList);  // 将label放到MessageList末尾
        else
            MessageList = label;
        // Y偏移
        auto oy = MessageY;
        if (IsEdit && AdjustEdit)
            oy = MessageY + Height;
        for (auto it = MessageList; it; oy += Height)
        {
            it->Y = oy;
            it = (TextLabelClass*)it->Get_Next();
        }
        ret = label;
    }
    return ret;
}
