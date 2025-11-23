#ifndef RE_MSGLIST_H
#define RE_MSGLIST_H

#include <hook/hook.h>
#include <msglist.h>

class TextLabelClass;
struct Re_MessageList : public MessageListClass
{
    using Self = Re_MessageList;

    int Init(
        int a2, int a3, int a4, int a5,
        int a6, int a7, int a8, bool a9,
        int a10, int a11, int a12
    );
    DEF_HOOKMEM(0x00572EA0, &Self::Init, false, H_Init)

    TextLabelClass* Add_Message(const char* a2, int a3, char* Source, ColorSchemeType cst, TextPrintType style, int a7);
    DEF_HOOKMEM(0x00572FE0, &Self::Add_Message, false, H_Add_Message)
};

#endif // !RE_MSGLIST_H