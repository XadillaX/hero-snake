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
     * @brief ����ǽ
     *
     * @param flag �Ƿ�Ϊǽ
     */
    void SetWall(bool flag = true)
    {
        walled = flag;
    }

    /**
     * @brief ���õ�ǰGUI
     *
     * @param flag �Ƿ�Ϊ��ǰGUI
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

    int x;                                      ///< �������
    int y;                                      ///< �������
    float rx;                                   ///< ��Ļ����
    float ry;                                   ///< ��Ļ����
    bool walled;                                ///< �Ƿ�Ϊǽ
    bool cur;                                   ///< �Ƿ�ǰGUI
    bool ycur;                                  ///< �Ƿ�ԭ��ǰGUI
    float ToYCURTimer;                          ///< ��Ϊ"ԭ��ǰGUI"��ʱ

    hgeGUI *parent;                             ///< ������������GUIϵͳ
};

#endif