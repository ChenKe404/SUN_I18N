#ifndef TSPP_EX_RESTATEMISSION_H
#define TSPP_EX_RESTATEMISSION_H

#include "msengine.h"
#include "textbtn.h"
#include "msfont.h"

struct MyButton : public TextButtonClass
{
    void Redraw();
};

struct RestateMission : public MSEngine
{
    virtual const char* Draw_Text(Surface*);

    bool Read_Mission_Ini(ScenarioClass* pSce);
    bool Start_Briefing_Screen(ScenarioClass* pSce);
    bool Wait_Resume();
    void Reset();

    ScenarioClass* pSce;
    char mText[1024];  // 简报文本
    const char* aText;      // 当前页文本
    Rect mRect;     // 文本区域
    int nX;
    int nY;
    MSFont* pFullfnt3;
    void* field_46C;
    MyButton* mBtnResume;
    DynamicVectorClass<MyButton*> vBtns;
};

#endif // TSPP_EX_RESTATEMISSION_H
