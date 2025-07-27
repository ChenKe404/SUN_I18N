#include "subtitle.h"

#include <fstream>
#include <fenv.h>
#include "resource.h"
#include <tibsun_globals.h>

static void trim(std::string& str)
{
	while (!str.empty() && str.back() == ' ')
		str.pop_back();
	while (!str.empty() && str.front() == ' ')
		str.erase(0,1);
}

static std::vector<std::string> split(const std::string& str, const std::string& sep)
{
	std::vector<std::string> ret;
	size_t left = 0, right = 0;
	do {
		right = str.find(sep, left);
		if (right == str.npos)
			right = str.size();
		auto t = str.substr(left, right - left);
		trim(t);
		if (!t.empty()) ret.emplace_back(std::move(t));
		left = right + 1;
	} while (right < str.size());
	return ret;
}

bool Subtitle::load(const std::string& file)
{
	d.clear();
	std::ifstream fi(file);
	if (!fi.is_open())
		return false;
	std::string line;

	size_t end_idx = 0;
	range rg{ 0,0 };
	bool got_time = false;
	int old_mode = fegetround();
	fesetround(FE_TONEAREST);
	while (std::getline(fi,line))
	{
		if (line.empty())
			continue;
		auto lead = line.front();
		if (lead == '%')	// 时间
		{
			line.erase(0, 1);
			auto s = split(line, ":");
			if (s.size() != 2)
				continue;
			rg.min = std::stod(s[0], &end_idx);
			rg.max = std::stod(s[1], &end_idx);
			if (rg.min == rg.max)
				continue;
			if (rg.min > rg.max)
				std::swap(rg.min, rg.max);
			got_time = true;
		}
		else if (lead == '>' && got_time)	// 文本
		{
			line.erase(0, 1);
			trim(line);
			d[rg] = line;
			got_time = false;
		}
	}
	fesetround(old_mode);
	return true;
}

void Subtitle::clear()
{
	d.clear();
}

std::vector<std::string*> Subtitle::fetch(double progress)
{
	std::vector<std::string*> ret;
	for (auto& it : d)
	{
		if (it.first.between(progress))
			ret.push_back(&it.second);
	}
	return ret;
}


SubtitleDrawer::SubtitleDrawer()
{
	_sur = nullptr;
	_rc = { 0,0,0,0 };
	_rc_clear = { 0,0,0,0 };
	_align_h = 0;
}

SubtitleDrawer::~SubtitleDrawer()
{
}

void SubtitleDrawer::load(const std::string& file)
{
	auto path = g_module_dir + "i18n/" + g_setting.loc + "/subtitles/";
	auto pos = file.find('.');
	if (pos != file.npos)
		path.append(file.substr(0, pos));
	else
		path.append(file);
	path.append(".cksub");
	_d.load(path);
}

void SubtitleDrawer::setAlignH(int align_h)
{
	_align_h = align_h;
}

void SubtitleDrawer::setSurface(DSurface* sur)
{
	_sur = sur;
}

void SubtitleDrawer::setMovieRect(const Rect& rc)
{
	_rc = rc;
	RECT crc;
	GetClientRect(MainWindow, &crc);
	auto w = crc.right - crc.left;
	auto h = crc.bottom - crc.top;
	if (rc.Width > w)
	{
		_rc.X = 0;
		_rc.Width = w;
	}
	if (rc.Height > h)
	{
		_rc.Y = 0;
		_rc.Height = h;
	}
}

void SubtitleDrawer::draw(double progress)
{
	using AL = MyFont::Align;
	using CharPtrs = MyFont::CharPtrs;
	if (!_sur || _rc.Width < 1 || _rc.Height < 1)
		return;

	auto texts = _d.fetch(progress);
	if (texts.empty())
		return;
	std::reverse(texts.begin(), texts.end());
	const auto width = _rc.Width - 20;

	std::u32string u32str;
	std::vector<std::pair<int,CharPtrs>> texts_chrs;
	Rect rc_text{ 0,0,0,0 };	// 上一块文本的矩形
	for (auto& it : texts)
	{
		u8_to_u32(it->c_str(), u32str);
		int oy = 0;
		if (rc_text.Height > 0)
			oy = rc_text.Height + 5 + texts_chrs.back().first;
		texts_chrs.emplace_back(oy, g_fnt_subtitle.cs(u32str));
		rc_text = g_fnt_subtitle.measure(texts_chrs.back().second, width);
	}
	
	auto& opt = g_fnt_subtitle.options();
	const auto old = opt.align;
	opt.align = AL::AL_BOTTOM | _align_h;
	int x = 0, y = 0;
	for (auto& it : texts_chrs)
	{
		x = _rc.X + 10;
		y = _rc.Y + _rc.Height - it.first - 10;
		if (_sur == g_SurPrimary)	// 主表面是屏幕表面, 所以绘制坐标需要相对窗口客户区起点
		{
			x += g_cliX;
			y += g_cliY;
		}
		auto& chrs = it.second;
		g_fnt_subtitle.draw(chrs.begin(), chrs.end(), _sur, x, y, width, -1, &_rc_clear);
	}
	opt.align = old;
}