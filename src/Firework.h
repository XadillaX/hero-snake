#pragma once
#include "hge.h"
#include "hgeparticle.h"
#include "hgesprite.h"
#include <Math.h>
#include "global.h"

enum firework_state { LAUNCH, FLIGHT, DETONATION, EXPLOSION, DEAD };

class Firework
{
public:
    Firework(HGE* hge, float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite, HEFFECT launch_sound, HEFFECT explosion_sound);
    ~Firework(void);
    void Tick(float dt);
    bool IsComplete() { return complete; };

private:
    firework_state current_state;
    float x,y;
    float launch_x, launch_y, detonation_x, detonation_y;
    float counter;
    float flight_time;
    DWORD color;
    hgeParticleSystem* rocket_particle_effect;
    hgeParticleSystem* explosion_particle_effect;
    hgeSprite* explosion_sprite;
    hgeSprite* rocket_sprite;
    const char* explosion_psi;
    const char* rocket_psi;
    bool complete;
    HEFFECT launch_sound;
    HEFFECT explosion_sound;
    HGE* hge;
};
