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
    int Type; //物品类型
    int x, y; //物品坐标
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
    HTEXTURE wall; //破墙
    HTEXTURE scissors; //减尾
    HTEXTURE clock; //减速
    HTEXTURE boom;  // Boom Boom Boom~
    HTEXTURE chgColor[5];   ///< 变色药水
    hgeSprite *wa; //破墙精灵
    hgeSprite *sc; //减尾精灵
    hgeSprite *cl; //减速精灵
    hgeSprite *bo; // Boom Boom Boom~精灵
    hgeSprite *chgCo[5];    ///< 变色药水精灵
    Stage *stg;
    Snake *snk[2];
    list<Stuff> l;
    int WallLeft[2]; //剩下可穿墙数量
};

#endif