#ifndef TSPP_EX_MSFONT_H
#define TSPP_EX_MSFONT_H

class ConvertClass;
struct MSFont
{
    virtual void Release(bool);

    virtual int Get_Char_Width(char);

    virtual int Get_Text_Width(const char*);

    virtual void Draw_Text(
        Surface* pSur, const char* chr,
        int X, int Y,
        int nFrame         // �ַ�����֡��
    );

    virtual void Draw_Char(
        Surface* pSur, char chr,
        int X, int Y,
        int nFrame,         // �ַ�����֡��
        bool bBleep         // �Ƿ������һ֡������Ч
    );

    void Get_Text_Rect(const char* lpText, Rect*);

    int paddingX;       // �ı���ˮƽ�߾�(��+��)
    int lineHeight;     // �����и�(�ַ������߶�)
    bool field_C;
    bool field_D;
    bool field_E;
    int field_10;
    uint8_t* pFntData;
    ConvertClass* field_18;
    bool bFntLoaded;
    bool field_1D;
    bool field_1E;
    bool field_1F;
};

#endif // !TSPP_EX_MSFONT_H
