#include "my_font.h"
#include <ddraw.h>
#include <font.h>
#include <drawer.h>
#include <blit/dsurface.h>
#include <tibsun/tibsun_globals.h>
#include <textprint.h>
#include <stdexcept>
#include <colorscheme.h>
#include <scenario.h>

/////////////////////////
/// FontDrawer

static inline uint16_t to_555(ck::color rgb)
{
	auto r = ck::cr(rgb) >> 3;
	auto g = ck::cg(rgb) >> 3;
	auto b = ck::cb(rgb) >> 3;
	return (r << 10) | (g << 5) | b;
}

static inline uint16_t to_565(ck::color rgb)
{
	auto r = ck::cr(rgb) >> 3;
	auto g = ck::cg(rgb) >> 2;
	auto b = ck::cb(rgb) >> 3;
	return (r << 11) | (g << 5) | b;
}

static void writePixel_16_565(uint8_t* mem, ck::color rgb)
{
	*((uint16_t*)mem) = to_565(rgb);
}

static void writePixel_16_555(uint8_t* mem, ck::color rgb)
{
	*((uint16_t*)mem) = to_555(rgb);
}

static void writePixel_24(uint8_t* mem, ck::color rgb)
{
	*mem = ck::cr(rgb);
	*(mem + 1) = ck::cg(rgb);
	*(mem + 2) = ck::cb(rgb);
}

static void writePixel_32(uint8_t* mem, ck::color rgb)
{
	*mem = 0xff;
	*(mem + 1) = ck::cr(rgb);
	*(mem + 2) = ck::cg(rgb);
	*(mem + 3) = ck::cb(rgb);
}

class FontDrawer : public ck::FontDrawer
{
	using Super = ck::FontDrawer;
	using Char = ck::Font::Char;
	using DataPtr = ck::Font::DataPtr;
public:
	FontDrawer()
		: writePixel(nullptr),
		_sur(nullptr),
		_mem(nullptr),
		_width(0),_height(0),
		_bytesPerPixel(0),
		_pitch(0)
	{}

	void setSurface(Surface* sur)
	{
		_sur = sur;
		if (!_sur) return;
		_width = _sur->Get_Width();
		_height = _sur->Get_Height();
		_bytesPerPixel = _sur->Get_Bytes_Per_Pixel();
		_pitch = sur->Get_Pitch();

		switch (_bytesPerPixel)
		{
		case 2:
			writePixel = writePixel_16_565;
			break;
		case 3:
			writePixel = writePixel_24;
			break;
		case 4:
			writePixel = writePixel_32;
			break;
		default: throw std::runtime_error("unsupported bytes per pixel!");
		}
	}

	bool lock() const
	{
		if (!_sur) return false;
		auto that = const_cast<FontDrawer*>(this);
		that->_mem = _sur->Lock(0, 0);
		return that->_mem != nullptr;
	}

	void unlock() const
	{
		_sur->Unlock();
	}

	Box draw(
		CharPtrList::const_iterator begin,
		CharPtrList::const_iterator end,
		int x, int y, int w, int h,
		const Options& opts
	) const override
	{
		if (!_font || !lock())
			return { 0,0,0,0 };

		auto box = Super::draw(begin, end, x, y, w, h, opts);

		unlock();
		return box;
	}

	Box draw(
		const ck::Font::CharPtrList& chrs,
		int x, int y,
		const Line& line,
		int spacingX
	) override
	{
		if (!_font || !lock())
			return { 0,0,0,0 };

		auto box = Super::draw(chrs, x, y, line, spacingX);

		unlock();
		return box;
	}

private:
	void perchar(int x, int y, const Char* chr, const DataPtr& d) const override
	{
		if (!writePixel)
			return;
		auto that = const_cast<FontDrawer*>(this);

		const auto w = d.w();
		const auto h = d.h();
		ck::color rgb;
		int ox, oy;
		for (int cy = 0;cy < d.h();++cy)
		{
			oy = y + cy;
			if (oy < 0 || oy > _height)    // 边界限定
				continue;
			for (int cx = 0;cx < d.w();++cx)
			{
				ox = x + cx;
				if (ox < 0 || ox > _width)     // 边界限定
					continue;
				rgb = d.get(cx, cy);
				if (rgb == _font->header().transparent)
					continue;
				auto mem = (uint8_t*)_mem + (y + cy) * _pitch + (ox) * _bytesPerPixel;	// 像素地址
				writePixel(mem, ck::mix(rgb, _mix, true));
			}
		}
	}
private:
	void (*writePixel)(uint8_t* mem, ck::color rgb);

	Surface* _sur;
	void* _mem;
	int _width, _height;
	int _bytesPerPixel;
	int _pitch;
};


/////////////////////////
/// MyFont

MyFont::MyFont()
	: _fnt(nullptr),
	_drawer(new FontDrawer)
{}

MyFont::MyFont(const MyFont& o)
	: _fnt(o._fnt),
	_drawer(new FontDrawer),
	_opts(o._opts)
{
	_drawer->setFont(_fnt);
}

MyFont::~MyFont()
{
	delete _drawer;
}

MyFont& MyFont::operator=(const MyFont& o)
{
	_fnt = o._fnt;
	_opts = o._opts;
	_drawer->setFont(_fnt);
	return *this;
}

void MyFont::attach(ck::Font* fnt)
{
	_fnt = fnt;
	_drawer->setFont(fnt);
}

int MyFont::lineHeight() const
{
	if (_fnt) return _fnt->header().lineHeight;
	return 0;
}

int MyFont::maxCharWidth() const
{
	if (_fnt) return _fnt->header().maxWidth;
	return 0;
}

int MyFont::charWidth(char32_t chr) const
{
	if (_fnt) return _fnt->c(chr).width;
	return 0;
}

MyFont::Options& MyFont::options()
{
	return _opts;
}

const MyFont::Char& MyFont::c(char32_t ch) const
{
	if (!_fnt) return {};
	return _fnt->c(ch);
}

MyFont::CharPtrs MyFont::cs(const char32_t* u32str) const
{
	if (!_fnt) return {};
	return _fnt->cs(u32str);
}

void MyFont::setMixColor(ck::color clr)
{
	_drawer->setMixColor(clr);
}

ck::color MyFont::mixColor() const
{
	return _drawer->mixColor();
}

Rect MyFont::measure(
	const CharPtrs& chrs,
	int maxWidth, int maxHeight,
	Lines* out_lines
)
{
	auto box = _drawer->measure(chrs, maxWidth, maxHeight, _opts, out_lines);
	return { box.x, box.y, box.w, box.h };
}

bool MyFont::draw(
	CharPtrs::const_iterator begin,
	CharPtrs::const_iterator end,
	Surface* pSur,
	int x, int y,
	int maxWidth,
	int maxHeight,
	Rect* out_rect
)
{
	_drawer->setSurface(pSur);
	ck::FontDrawer::Box box{ 0,0,0,0 };
	box = _drawer->draw(begin, end, x, y, maxWidth, maxHeight, _opts);
	if (out_rect) *out_rect = { box.x,box.y,box.w,box.h };
	return false;
}

bool MyFont::draw(
	const CharPtrs& chrs,
	const Line& line,
	Surface* pSur,
	int x, int y,
	Rect* out_rect
)
{
	_drawer->setSurface(pSur);
	auto spc_x = _drawer->font()->header().spacingX;
	auto box = _drawer->draw(chrs, x, y, line, spc_x);
	if (out_rect) *out_rect = { box.x,box.y,box.w,box.h };
	return false;
}

MyFont* font_ptr(int style)
{
	switch (style & 0xF)
	{
	case 2:	// 8pt
	case 6:	// grad6fnt
	//case 7:	// kia6pt
	case 8:	// 12pt
		return &g_fnt12o;
		//case 9:	// editfnt
		break;
	//default: 6pt
	}
	return &g_fnt12;
}

Point2D MYSimple_Text_Print(const char32_t* u32str, XSurface* pSur, Rect* pRect, Point2D* pPt, ColorScheme* cs, int nStyle)
{
	Point2D pt{ 0,0 };
	if (!u32str)
		return pt;

	using AL = MyFont::Align;

	int x = pRect->X + pPt->X;
	int y = pRect->Y + pPt->Y;

	auto fnt = font_ptr(nStyle);
	auto chrs = fnt->cs(u32str);

	auto& opt = fnt->options();
	auto old = opt.align;
	nStyle = nStyle & 0x300;
	if (nStyle == TPF_CENTER)
		opt.align = AL::AL_HCENTER;
	else if (nStyle == TPF_RIGHT)
		opt.align = AL::AL_RIGHT;
	else
		opt.align = AL::AL_LEFT;

	Rect rc;

	auto old_mix = fnt->mixColor();
	fnt->setMixColor(to_color(cs->HSV));
	fnt->draw(chrs, pSur, x, y, -1, -1, &rc);
	fnt->setMixColor(old_mix);

	opt.align = old;
	pt = { rc.X,rc.Y };
	return pt;
}
