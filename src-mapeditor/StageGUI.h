#ifndef STAGEGUI_H
#define STAGEGUI_H
#pragma once
#include <hge.h>
#include <hgesprite.h>
#include <hgegui.h>

#define SAFEDEL(p) { if(p) { delete p; p = NULL; } }
extern int mat[15][20];
extern bool saved;
extern HEFFECT hEff;

class StageGUI : public hgeGUIObject
{
    friend void _floodfill(int x, int y, int flag, hgeGUI *gui);
    friend void New_Func();
    friend void Open_Func();
public:
    ~StageGUI(void);
    StageGUI(int _id, int x, int y, hgeGUI *par, bool walled = false);
    virtual void Render();
    virtual void Update(float dt);
    virtual void MouseOver(bool bOver);
    virtual bool MouseLButton(bool bDown);
    virtual bool MouseRButton(bool bDown);

    /**
     * @brief 设置墙
     *
     * @param flag 是否为墙
     */
    void SetWall(bool flag = true)
    {
        walled = flag;
    }

    /**
     * @brief 设置当前GUI
     *
     * @param flag 是否为当前GUI
     */
    void SetCurrent(bool flag = true)
    {
        cur = flag;
        ycur = false;
        ToYCURTimer = 0.0f;
        if(flag) hge->Effect_Play(hEff);
    }

private:
    StageGUI(void);
    void _SetCurrent();

    int x;                                      ///< 横轴计数
    int y;                                      ///< 纵轴计数
    float rx;                                   ///< 屏幕横轴
    float ry;                                   ///< 屏幕纵轴
    bool walled;                                ///< 是否为墙
    bool cur;                                   ///< 是否当前GUI
    bool ycur;                                  ///< 是否原当前GUI
    float ToYCURTimer;                          ///< 成为"原当前GUI"延时

    hgeGUI *parent;                             ///< 父对象，所属的GUI系统
};

#endif