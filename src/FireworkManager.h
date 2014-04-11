#pragma once
#include "hge.h"
#include <vector>
#include <algorithm>
#include "Firework.h"
#include "global.h"

class FireworkManager
{
public:
    FireworkManager(HGE* hge, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite);
    ~FireworkManager(void);
    void AddFirework(float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color);
    void RemoveFirework(Firework* firework);
    void Tick();
    int  GetAlive() { return fireworks.size(); };

private:
    HGE* hge;
    std::vector<Firework*> fireworks;
    hgeSprite* explosion_sprite;
    hgeSprite* rocket_sprite;
    HEFFECT firework_launch;
    HEFFECT firework_explosion;
    const char* rocket_psi;
    const char* explosion_psi;
};
