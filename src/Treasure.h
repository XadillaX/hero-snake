#pragma once

#ifndef TREASURE_H
#define TREASURE_H

#include <hge.h>
#include <hgesprite.h>
#include <list>
#include "Snake.h"
#include "Stage.h"

using namespace std;
class Stage;
struct Stuff {
    int Type; //��Ʒ����
    int x, y; //��Ʒ����
    float NowTimer, MaxTimer;
};

class Treasure {
    friend class Snake;
    friend bool RenderFunc();
    friend bool RunningFunc();
    friend void DRunningRender();
public:
    Treasure(HGE*, Stage*);
    void AddTreasure();
    void ClearTreasure();
    void Update(float);
    void Render();
    int DoEffect(int, int, int id = 0);
    void BindSnake(Snake*, int id = 0);
    ~Treasure();
private:
    float NowTimer, MaxTimer;
    HGE* hge;
    HTEXTURE wall; //��ǽ
    HTEXTURE scissors; //��β
    HTEXTURE clock; //����
    HTEXTURE boom;  // Boom Boom Boom~
    HTEXTURE chgColor[5];   ///< ��ɫҩˮ
    hgeSprite *wa; //��ǽ����
    hgeSprite *sc; //��β����
    hgeSprite *cl; //���پ���
    hgeSprite *bo; // Boom Boom Boom~����
    hgeSprite *chgCo[5];    ///< ��ɫҩˮ����
    Stage *stg;
    Snake *snk[2];
    list<Stuff> l;
    int WallLeft[2]; //ʣ�¿ɴ�ǽ����
};

#endif