#ifndef TSPP_EX_GLOBAL_H
#define TSPP_EX_GLOBAL_H

#include <tspp.h>
#include <blit/dsurface.h>
#include <wwmouse.h>
#include <wwkeyboard.h>
#include <palette.h>

ARRAY_DEC(char, g_aPcxScore, 16);
ARRAY_DEC(char, g_aAudBleep1, 16);
ARRAY_DEC(char, g_aAudText1, 16);
ARRAY_DEC(int, g_pAudText1A, 5);
ARRAY_DEC(int, g_pAudText1B, 5);

extern bool& g_bSlotText;
extern bool& g_bWindowed;
extern char& g_Delimiter;
extern int& g_WndWidth;
extern int& g_WndHeight;

extern float& g_FltAudSpeed;

extern WWMouseClass*& g_Mouse;			// 鼠标
extern WWKeyboardClass*& g_Keyboard;	// 键盘

extern DSurface*& g_SurPrimary;		// 主表面
extern DSurface*& g_SurHidden;		// 后台表面
extern DSurface*& g_SurAlternate;	// 备用表面
extern DSurface*& g_SurSidebar;		// 面板表面
extern DSurface*& g_SurLogic;		// 逻辑表面
extern DSurface*& g_SurComposite;	// 复合表面
extern DSurface*& g_SurTile;		// 地块表面

extern PaletteClass& g_PalOriginal;

#endif // !TSPP_EX_GLOBAL_H
