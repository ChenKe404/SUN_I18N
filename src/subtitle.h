#ifndef SUBTITLE_H
#define SUBTITLE_H

class Subtitle
{
public:
	struct range {
		double min, max;

		inline bool between(double v) const {
			return min <= v && v <= max;
		}
		inline bool operator <(const range& o) const {
			return min < o.min;
		}
	};
public:
	bool load(const std::string& file);
	void clear();
	std::vector<std::string*> fetch(double progress);
private:
	std::map<range, std::string> d;
};

class SubtitleDrawer
{
public:
	SubtitleDrawer();
	~SubtitleDrawer();
	void load(const std::string& file);

	// ����ˮƽ����
	void setAlignH(int align_h);
	void setSurface(DSurface* sur);
	// ���õ�Ӱ�ľ�������
	void setMovieRect(const Rect&);

	void draw(double progress);
private:
	Rect _rc_clear;
	Rect _rc;
	int _align_h;
	DSurface* _sur;
	Subtitle _d;
};

#endif // !SUBTITLE_H