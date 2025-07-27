#ifndef TSPP_EX_SCOREFONT_H
#define TSPP_EX_SCOREFONT_H

struct ScoreFontClass
{
    virtual void Release(bool);

    virtual int Get_Char_Width(char chr);

    virtual int Get_Text_Width(const char* lpText);

    virtual bool Draw_Text(
        Surface* pSur, const char* lpText,
        int X, int Y,
        int nFrame
    );

    virtual void Draw_Char(
        Surface* pSur, char chr,
        int X, int Y,
        int nFrame,
        bool bBleep
    );

    int field_0;
    int field_8;
    uint8_t* pFntData;
    bool bFntLoaded;
    bool field_11;
    ConvertClass* pDrawer;
};

struct ScoreFullFontClass : public ScoreFontClass
{
    void Release(bool) override;
};

struct ScoreBigFontClass : public ScoreFontClass
{
    void Release(bool) override;
};

#endif // !TSPP_EX_SCOREFONT_H
