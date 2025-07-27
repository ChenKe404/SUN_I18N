#ifndef TSPP_EX_SCOREANIM_H
#define TSPP_EX_SCOREANIM_H

#include <convert.h>
#include <surface.h>

struct ScoreFullFontClass;
struct ScoreAnimClass
{
    virtual ~ScoreAnimClass();
    virtual int Get_Elapsed_Time();
    virtual void Update_Time();
    virtual bool Draw(int nFrame) = 0;

    int nX, nY;
    int nTime;
    void* pObj;
    int nInterval;
    const char* aText;
    int nCurFrame;          // 当前帧数
    int nFrame;             // 动画帧数
    bool bFlag;
    ScoreFullFontClass* pFullFnt;
};

// 打印任务结束的分数统计界面的动画
struct ScoreTimeClass : public ScoreAnimClass
{
    virtual ~ScoreTimeClass();
    bool Draw(int nFrame) override;
};

// 打印任务结束的分数统计界面的文本
struct ScorePrintClass : public ScoreAnimClass
{
    virtual ~ScorePrintClass();
    bool Draw(int nFrame) override;
};

#endif // TSPP_EX_SCOREANIM_H
