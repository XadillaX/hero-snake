#include "firework.h"

Firework::Firework(HGE* hge, float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite, HEFFECT launch_sound, HEFFECT explosion_sound)
{
    this->explosion_psi = explosion_psi;
    this->rocket_psi = rocket_psi;
    this->rocket_sprite = rocket_sprite;
    this->explosion_sprite = explosion_sprite;
    this->launch_x = launch_x;
    this->launch_y = launch_y;
    this->detonation_x = detonation_x;
    this->detonation_y = detonation_y;
    this->color = color;
    this->x = launch_x;
    this->y = launch_y;
    this->flight_time = 1.7; // time from launch to detonation in seconds
    this->launch_sound = launch_sound;
    this->explosion_sound = explosion_sound;

    this->hge = hge;

    current_state = LAUNCH;
    counter = 0;
    complete = false;

}

Firework::~Firework(void)
{
    delete rocket_particle_effect;
    delete explosion_particle_effect;
}

void Firework::Tick(float dt)
{
    switch (current_state)
    {
    case LAUNCH:
        rocket_particle_effect = new hgeParticleSystem(rocket_psi,rocket_sprite);
        rocket_particle_effect->info.bRelative = true;
        rocket_particle_effect->MoveTo(launch_x,launch_y);
        rocket_particle_effect->Fire();
        current_state = FLIGHT;
        hge->Effect_PlayEx(launch_sound, EffectVol, 0, 1.0F, false);
        //hge->Effect_Play(launch_sound);
        break;
            
    case FLIGHT:
        counter += dt;
        if (counter < flight_time)
        {
            x += ((detonation_x - launch_x)/flight_time) * dt;
            y += ((detonation_y - launch_y)/flight_time) * dt;
            rocket_particle_effect->Update(dt);
            rocket_particle_effect->MoveTo(x,y,false);
            rocket_particle_effect->Render();
        }
        else current_state = DETONATION;
        break;
        
    case DETONATION:
        explosion_sprite->SetBlendMode(BLEND_COLORMUL | BLEND_ALPHABLEND | BLEND_NOZWRITE);
        explosion_particle_effect = new hgeParticleSystem(explosion_psi,explosion_sprite);
        explosion_particle_effect->info.colColorStart = this->color;
        explosion_particle_effect->info.colColorEnd = this->color - 0xCC000000;
        explosion_particle_effect->MoveTo(detonation_x,detonation_y);
        explosion_particle_effect->Fire();
        current_state = EXPLOSION;
        hge->Effect_PlayEx(explosion_sound, EffectVol, 0, 1.0F, false);
        //hge->Effect_Play(explosion_sound);
        break;
        
    case EXPLOSION:
        explosion_particle_effect->Update(dt);
        explosion_particle_effect->Render();
        if (!(explosion_particle_effect->GetParticlesAlive() > 0))
        {
            complete = true;
            current_state = DEAD;
        }
        break;

    case DEAD: break;

    }
}