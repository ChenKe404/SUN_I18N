#include "definitions.h"
#include "anim.h"
#include "scorefont.h"
#include "scoreanim.h"
#include "scorescreen.h"
#include "restatemission.h"

ARRAY_DEF(0x00709744, char, g_aPcxScore, 16);
ARRAY_DEF(0x00705440, char, g_aAudBleep1, 16);
ARRAY_DEF(0x0070527C, char, g_aAudText1, 16);
ARRAY_DEF(0x008072D0, int, g_pAudText1A, 5);
ARRAY_DEF(0x0080A380, int, g_pAudText1B, 5);

bool& g_bSlotText = Make_Global<bool>(0x0074A01C);
bool& g_bWindowed = Make_Global<bool>(0x007A1790);
bool& g_HighResolution = Make_Global<bool>(0x007E4745);
char& g_Delimiter = Make_Global<char>(0x00709740);

int& g_WndWidth = Make_Global<int>(0x0074C268);
int& g_WndHeight = Make_Global<int>(0x0074C26C);

float& g_FltAudSpeed = Make_Global<float>(0x007E4748);

WWMouseClass*& g_Mouse = Make_Global<WWMouseClass*>(0x0074C8F0);
WWKeyboardClass*& g_Keyboard = Make_Global<WWKeyboardClass*>(0x007482C0);

DSurface*& g_SurPrimary = Make_Global<DSurface*>(0x0074C5D8);
DSurface*& g_SurHidden = Make_Global<DSurface*>(0x0074C5DC);
DSurface*& g_SurAlternate = Make_Global<DSurface*>(0x0074C5E0);
DSurface*& g_SurSidebar = Make_Global<DSurface*>(0x0074C5D0);
DSurface*& g_SurLogic = Make_Global<DSurface*>(0x0074C5E4);
DSurface*& g_SurComposite = Make_Global<DSurface*>(0x0074C5EC);
DSurface*& g_SurTile = Make_Global<DSurface*>(0x0074C5CC);

PaletteClass& g_PalOriginal = Make_Global<PaletteClass>(0x0074B338);
ColorScheme**& g_ArrCS = Make_Global<ColorScheme**>(0x0080A2CC);

DEFINE_IMPLEMENTATION(int GetTime(), 0x0060AB10)

DEFINE_IMPLEMENTATION(Rect* Fancy_Text_Print(
    Rect* pRect, const char* aFormat, Surface* pSur,
    Point2D* pPt, Point2D* pPt1,
    ColorScheme* pCS,
    int a7, TextPrintType tpt, ...
), 0x00474E70)

DEFINE_IMPLEMENTATION(LONG Ctrl_Draw_Text(
    COLORREF color,
    void* a2,
    Rect* pRect,
    LPCSTR lpString,
    int length,
    int align_h,
    int align_v,
    XSurface* a8
), 0x0059F1D0)

DEFINE_IMPLEMENTATION(void MSFont::Release(bool), 0x005727B0)
DEFINE_IMPLEMENTATION(int MSFont::Get_Char_Width(char), 0x00572400)
DEFINE_IMPLEMENTATION(int MSFont::Get_Text_Width(const char*), 0x005723B0)

DEFINE_IMPLEMENTATION(void MSFont::Draw_Text(
    Surface*, const char*,
    int, int,
    int
), 0x00572640)

DEFINE_IMPLEMENTATION(void MSFont::Draw_Char(
    Surface*, char,
    int, int,
    int, bool
), 0x005724D0)

DEFINE_IMPLEMENTATION(void MSFont::Get_Text_Rect(
    const char*, Rect*
), 0x00572310)

DEFINE_IMPLEMENTATION(int Fit_Text(
    char*, MSFont*, int
), 0x0056C990)

DEFINE_IMPLEMENTATION(int More_Page(
    char*, MSFont*, int
), 0x0056CBE0)

MSAnim::MSAnim()
    : nX(0),nY(0),bPlaying(false),nTime(0),field_14(0),nInterval(0)
{
    *((unsigned long*)this) = (unsigned long)0x006D480C;
}
DEFINE_IMPLEMENTATION_DESTRUCTOR(MSAnim::~MSAnim(), 0x0056DF00)
DEFINE_IMPLEMENTATION(bool MSAnim::Set_Playing(bool), 0x004B9040)
DEFINE_IMPLEMENTATION(int MSAnim::Get_Elapsed_Time(), 0x004B9050)
DEFINE_IMPLEMENTATION(void MSAnim::Update_Time(), 0x004B90A0)
DEFINE_IMPLEMENTATION(bool MSAnim::func7(), 0x0056DEF0)

DEFINE_IMPLEMENTATION_CONSTRUCTOR(MSPrintAnim::MSPrintAnim(
    const char*,
    int, int,
    MSFont*,
    Rect*,
    int, int,
    bool, bool
), 0x0056C6F0)
DEFINE_IMPLEMENTATION_DESTRUCTOR(MSPrintAnim::~MSPrintAnim(), 0x0056A980)
DEFINE_IMPLEMENTATION(bool MSPrintAnim::Draw_Frame(XSurface*, Rect *), 0x0056CC40)
DEFINE_IMPLEMENTATION(int MSPrintAnim::Refresh(XSurface*,const Rect*), 0x0056CE60)
DEFINE_IMPLEMENTATION(Rect* MSPrintAnim::Get_Rect(Rect*), 0x0056D1E0)
DEFINE_IMPLEMENTATION(bool MSPrintAnim::func7(), 0x0056D230)

MSWordAnim::MSWordAnim(
    const char* lpText,
    int X, int Y,
    MSFont* pFnt3,
    Rect* pRect,
    int nInterval,
    int nCount1,
    bool bFlag,
    bool bSingleLine
) : MSPrintAnim(lpText, X, Y, pFnt3, pRect, nInterval, nCount1, bFlag, bSingleLine),
    nOY(nY), mRectLine{ 0,0,0,0 }
{
    *((unsigned long*)this) = (unsigned long)0x006D47BC;
}
DEFINE_IMPLEMENTATION_DESTRUCTOR(MSWordAnim::~MSWordAnim(), 0x0056A950)
DEFINE_IMPLEMENTATION(bool MSWordAnim::Draw_Frame(XSurface*, Rect *), 0x0056CFD0)
DEFINE_IMPLEMENTATION(int MSWordAnim::Refresh(XSurface*,const Rect*), 0x0056D290)



DEFINE_IMPLEMENTATION(ScoreAnimClass::~ScoreAnimClass(), 0x005E74A0)
DEFINE_IMPLEMENTATION(int ScoreAnimClass::Get_Elapsed_Time(), 0x005E7430)
DEFINE_IMPLEMENTATION(void ScoreAnimClass::Update_Time(), 0x005E7480)

DEFINE_IMPLEMENTATION(ScoreTimeClass::~ScoreTimeClass(), 0x005E74C0)
DEFINE_IMPLEMENTATION(bool ScoreTimeClass::Draw(int), 0x005E6FA0)

DEFINE_IMPLEMENTATION(ScorePrintClass::~ScorePrintClass(), 0x005E74A0)
DEFINE_IMPLEMENTATION(bool ScorePrintClass::Draw(int), 0x005E74E0)


DEFINE_IMPLEMENTATION(void ScoreFontClass::Release(bool), 0x005E7500)
DEFINE_IMPLEMENTATION(int ScoreFontClass::Get_Char_Width(char), 0x005E6A70)
DEFINE_IMPLEMENTATION(int ScoreFontClass::Get_Text_Width(const char*), 0x005E6A40)
DEFINE_IMPLEMENTATION(bool ScoreFontClass::Draw_Text(Surface *,const char*,int,int,int), 0x005E6C80)
DEFINE_IMPLEMENTATION(void ScoreFontClass::Draw_Char(Surface *,char,int,int,int,bool), 0x005E6B30)

DEFINE_IMPLEMENTATION(void ScoreFullFontClass::Release(bool), 0x005E7520)

DEFINE_IMPLEMENTATION(void ScoreBigFontClass::Release(bool), 0x005E7550)

DEFINE_IMPLEMENTATION(void MyButton::Redraw(), 0x005C1130)

DEFINE_IMPLEMENTATION(const char* RestateMission::Draw_Text(Surface*), 0x005C0EC0)
DEFINE_IMPLEMENTATION(bool RestateMission::Read_Mission_Ini(ScenarioClass*), 0x005C0860)
DEFINE_IMPLEMENTATION(bool RestateMission::Start_Briefing_Screen(ScenarioClass* pRem), 0x005C0440)
DEFINE_IMPLEMENTATION(bool RestateMission::Wait_Resume(), 0x005C0F10)
DEFINE_IMPLEMENTATION(void RestateMission::Reset(), 0x005C0E40)

DEFINE_IMPLEMENTATION(void ScoreScreen::Start_Score_Screen(), 0x005E2EA0)
DEFINE_IMPLEMENTATION(int ScoreScreen::Play_Anim(int), 0x005E63A0)




