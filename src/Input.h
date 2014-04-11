#pragma once

#ifndef INPUT_H
#define INPUT_H
//Small Input textfield 

#include <hge.h>
#include <hgefont.h>
#include <hgecolor.h>
#include <hgegui.h>
#include <hgeguictrls.h>
#include <cstdio>
#include <string>
#include "global.h"

class Forminput : public hgeGUIObject{ 

public: 
    Forminput(int _id, hgeFont* _fnt, HEFFECT snd, float xPos, float yPos, 
        float length, char* prevalue); 
    ~Forminput(); 
    virtual void Render(); 
    //void Render(float x, float y); 
    virtual void Update(float dt);
    virtual void Enter();
    virtual void Leave();
    virtual bool IsDone();
    virtual bool MouseLButton(bool bDown); 
    virtual void MouseOver(bool bOver);
    virtual void Focus(bool bFocused); 
    virtual bool KeyClick(int key, int chr); 
    //virtual void Update(float fDeltaTime); 
    void setText(char *newText); 
    char *getText(); 

protected: 
    int caretposition; 
    char* caret; 
    bool caretVisible; //for blinking effect 
    bool focused; 
    float caretTimer; //for blinking effect 
    const static int maxchar = 30; 
    float xPos; 
    float yPos; 
    float delay;
    hgeFont *font; 
    HEFFECT snd;
    float length; 
    char *content; 

    hgeColor    scolor, dcolor, scolor2, dcolor2, sshadow, dshadow;
    hgeColor    color, shadow;
    float        soffset, doffset, offset;
    float        timer, timer2;
}; 
#endif