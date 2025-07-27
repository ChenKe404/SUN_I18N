#include <colorscheme.h>
#include <textprint.h>
#include <tspp_ex/definitions.h>

#define SUN_I18N_VERSION "SUN I18N PATCH PROJECT V" PROJECT_VERSION " By ChenKe404"

static Rect* __stdcall fancy_text_print(Rect* pRect, const char* lpText, Surface* pSur, Point2D* pPt, Point2D* pPt1, ColorScheme* pCS, unsigned a7, TextPrintType tpt, ...)
{
	Rect rc = *pRect;
	Point2D pt{ 2,pPt1->Y };
	Fancy_Text_Print(&rc, SUN_I18N_VERSION, pSur, pPt, &pt, pCS, a7, tpt &~ TPF_RIGHT);
	Fancy_Text_Print(pRect, lpText, pSur, pPt, pPt1, pCS, a7, tpt);
	pRect->X = 0;
	return pRect;
}

void init_patch_version()
{
	write_cmd(0x004E5597, fancy_text_print, OP_CALL);
}