#ifndef TSPP_EX_ANIM_H
#define TSPP_EX_ANIM_H

#include "restatemission.h"
#include "definitions.h"

struct MSAnim
{
    MSAnim();
    virtual ~MSAnim();
    virtual bool Set_Playing(bool);
    virtual int Get_Elapsed_Time();
    virtual void Update_Time();

    /**
     * @brief 绘制当前帧
     * @pRect  输出需要刷新的范围
     * @return 是否播放完毕
     */
    virtual bool Draw_Frame(XSurface*, Rect* pRect) = 0;

    /**
     * @brief 刷新
     * @return
     */
    virtual int Refresh(XSurface*,const Rect*) = 0;
    virtual Rect* Get_Rect(Rect*) = 0;
    virtual bool func7();

    int nX;             // 绘制起点X
    int nY;             // 绘制起点Y
    bool bPlaying;      // 正在播放
    int nTime;
    int field_14;
    int nInterval;      // 帧播放时间间隔
};

struct MSPrintAnim : public MSAnim
{
    MSPrintAnim(
        const char* lpText,
        int X, int Y,
        MSFont* pFnt3,
        Rect* pRect,
        int nInterval,
        int nCount1,
        bool bFlag,
        bool bSingleLine
    );

    virtual ~MSPrintAnim();

    bool Draw_Frame(XSurface*, Rect* pRect) override;

    /**
     * @brief Refresh
     * @return 绘制成功的字符数
     */
    int Refresh(XSurface*,const Rect*) override;
    Rect* Get_Rect(Rect* pREct) override;
    bool func7() override;



    const char* aText;  // 待绘制文本
    int nStart;         // 文本起始下标
    int nFrame;         // 帧数
    int nNul;           // 本中的空字符数
    int nCount1;
    bool bNext;         // 继续下一帧
    MSFont* pFnt3;
    Rect mRectBound;    // 文本绘制的限制区域
    bool bFlag_2;
    bool bSingleLine;
    bool field_4A;
    bool field_4B;
    int nOX;            // 横向偏移
};

struct MSWordAnim : public MSPrintAnim
{
    MSWordAnim(
        const char* lpText,
        int X, int Y,
        MSFont* pFnt3,
        Rect* pRect,
        int nInterval,
        int nCount1,
        bool bFlag,
        bool bSingleLine
    );

    ~MSWordAnim();

    bool Draw_Frame(XSurface*, Rect *) override;

    /**
     * @brief 刷新之前绘制的高亮文本
     * @return 当前文本下标
     */
    int Refresh(XSurface*,const Rect*) override;

    int nOY;            // 纵向偏移
    Rect mRectLine;     // 当前绘制的文本行区域
};

struct MSPCXAnim : public MSAnim
{
    bool Draw_Frame(XSurface*, Rect *) override{ return true; }
    int Refresh(XSurface*,const Rect*) override { return 0; }

    int filed_1C;
    int field_20;
    int field_24;
    int field_28;
    int field_2C;
    int field_30;
    int field_34;
    int field_38;
};

#endif // TSPP_EX_ANIM_H
