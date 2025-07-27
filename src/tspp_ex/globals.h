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

extern WWMouseClass*& g_Mouse;			// ���
extern WWKeyboardClass*& g_Keyboard;	// ����

extern DSurface*& g_SurPrimary;		// ������
extern DSurface*& g_SurHidden;		// ��̨����
extern DSurface*& g_SurAlternate;	// ���ñ���
extern DSurface*& g_SurSidebar;		// ������
extern DSurface*& g_SurLogic;		// �߼�����
extern DSurface*& g_SurComposite;	// ���ϱ���
extern DSurface*& g_SurTile;		// �ؿ����

extern PaletteClass& g_PalOriginal;

#endif // !TSPP_EX_GLOBAL_H
