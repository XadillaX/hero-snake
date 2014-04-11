#include "fireworkmanager.h"

FireworkManager::FireworkManager(HGE* hge, const char* rocket_psi, const char* explosion_psi, hgeSprite* rocket_sprite, hgeSprite* explosion_sprite)
{
    this->hge = hge;
    this->explosion_sprite = explosion_sprite;
    this->rocket_sprite = rocket_sprite;
    this->explosion_psi = explosion_psi;
    this->rocket_psi = rocket_psi;
    this->firework_launch = hge->Effect_Load("sound\\fireworklaunch.ogg");
    this->firework_explosion = hge->Effect_Load("sound\\fireworkexplosion.ogg");
}

FireworkManager::~FireworkManager(void)
{
    hge->Effect_Free(firework_launch);
    hge->Effect_Free(firework_explosion);
    for (unsigned int i=0; i<fireworks.size(); i++) RemoveFirework(fireworks[i]);
}

void FireworkManager::RemoveFirework(Firework* firework)
{
    using std::vector;
    vector<Firework*>::iterator firework_iterator;
    firework_iterator = find(fireworks.begin(), fireworks.end(), firework);
    if (firework_iterator != fireworks.end())
    {
        delete firework;
        fireworks.erase(firework_iterator);
    }
}

void FireworkManager::AddFirework(float launch_x, float launch_y, float detonation_x, float detonation_y, DWORD color)
{
    fireworks.push_back(new Firework(hge, launch_x, launch_y, detonation_x, detonation_y, color, this->rocket_psi, this->explosion_psi, this->rocket_sprite, this->explosion_sprite, this->firework_launch, this->firework_explosion));
}

void FireworkManager::Tick()
{

    for (unsigned int i=0; i<fireworks.size(); i++)
    {
        if (fireworks[i]->IsComplete())    RemoveFirework(fireworks[i]);
        else fireworks[i]->Tick(hge->Timer_GetDelta());
    }
}
