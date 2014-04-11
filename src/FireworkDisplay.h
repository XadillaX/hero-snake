#pragma once
#include "Firework.h"
#include "FireworkManager.h"
#include <vector>
#include <algorithm>
#include "hge.h"


struct display_entry {
    float launch_x;
    float launch_y;
    float detonation_x;
    float detonation_y;
    DWORD color;
    float time;
};

class FireworkDisplay
{
public:
    FireworkDisplay( HGE* hge, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite);
    ~FireworkDisplay(void);
    void AddFirework(float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color, float time);
    void RemoveEntry(display_entry* entry);
    void Play(void);
    void Tick(void);
    bool IsComplete(void);
private:
    std::vector<display_entry*> display;
    FireworkManager* fireworks;
    float start_time;
    float current_time;
    HGE* hge;
};
