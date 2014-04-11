#include "fireworkdisplay.h"

FireworkDisplay::FireworkDisplay( HGE* hge, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite)
{
    start_time = 0;
    this->hge = hge;
    fireworks = new FireworkManager(hge, rocket_psi, explosion_psi, rocket_sprite, explosion_sprite);
}

FireworkDisplay::~FireworkDisplay(void)
{
    delete fireworks;
}

void FireworkDisplay::AddFirework(float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color, float time)
{
    display_entry* new_firework = new display_entry();
    new_firework->launch_x = launch_x;
    new_firework->launch_y = launch_y;
    new_firework->detonation_x = detonation_x;
    new_firework->detonation_y = detonation_y;
    new_firework->color = color;
    new_firework->time = time;
    display.push_back(new_firework);
}

void FireworkDisplay::RemoveEntry(display_entry* entry)
{

    using std::vector;
    vector<display_entry*>::iterator event_iterator;
    event_iterator = find(display.begin(), display.end(), entry);
    if (event_iterator != display.end()) display.erase(event_iterator);

}

void FireworkDisplay::Play(void)
{
    start_time = hge->Timer_GetTime();
    current_time = 0;
}

void FireworkDisplay::Tick(void)
{
    current_time += hge->Timer_GetDelta();
    for (unsigned int i=0; i<display.size(); i++)
    {
        if (current_time > display[i]->time)
        {
            fireworks->AddFirework(display[i]->launch_x, display[i]->launch_y, display[i]->detonation_x, display[i]->detonation_y, display[i]->color);
            this->RemoveEntry(display[i]);
        }
    }
    fireworks->Tick();
}

bool FireworkDisplay::IsComplete(void)
{
    return (display.size() == 0 && fireworks->GetAlive() == 0);
}
