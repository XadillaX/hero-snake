#include "Input.h"

Forminput::Forminput(int _id, hgeFont* _fnt, HEFFECT _snd, float _xPos, float _yPos, 
    float _delay, char *prevalue){ 
        id = _id; 
        font = _fnt; 
        snd = _snd;
        delay = _delay;
        xPos = _xPos; 
        yPos = _yPos; 
        bStatic = false; 
        bVisible = true; 
        bEnabled = true; 
        caretVisible = true; 
        focused = false; 
        caretTimer = 0.0f; 
        caret = "_"; 
        content = new char[maxchar+1]; //maxchar is defined in the header 
        strncpy(content, prevalue, maxchar); 
        int prevalue_length = (int)strlen(prevalue); 
        if (prevalue_length < maxchar){ 
            caretposition = prevalue_length; 
        } 
        else{ 
            caretposition = maxchar-1; 
        } 
        content[caretposition] = '\0'; 
        content[maxchar+1] = '\0'; //Last char will always be a string terminator 

        color.SetHWColor(0xFF7A1494);//(0xFFFFE060);
        shadow.SetHWColor(0x30000000);//(0x30000000);
        offset=0.0f;
        timer=-1.0f;
        timer2=-1.0f;
        //float w = font->GetStringWidth(prevalue);
        //rect.Set(_xPos-w/2, _yPos, _xPos+w/2, _yPos+font->GetHeight());
        length = font->GetStringWidth(content) + 32;
        rect.Set(xPos - length, yPos, xPos+length, yPos+(font->GetHeight())); 
}; 

Forminput::~Forminput(){ 
    delete font; 
} 

void Forminput::Render(){ 
    float w = font->GetStringWidth(content);
    font->SetColor(shadow.GetHWColor());
    font->Render(xPos-w/2+offset+3, yPos+3, HGETEXT_LEFT, content); 
    font->SetColor(color.GetHWColor());
    font->Render(xPos-w/2-offset, yPos-offset, HGETEXT_LEFT, content);
    //Renders the caret 
    if (focused && caretVisible){ 
        float width = font->GetStringWidth(content); 
        font->SetColor(shadow.GetHWColor());
        font->Render(xPos-w/2+width+offset+3, yPos+3, HGETEXT_LEFT, caret); 
        font->SetColor(color.GetHWColor());
        font->Render(xPos-w/2+width-offset, yPos-offset, HGETEXT_LEFT, caret);
    } 
} 

// This method is called each frame,
// we should update the animation here
void Forminput::Update(float dt) {
    if(timer2 != -1.0f) {
        timer2+=dt;
        if(timer2 >= delay+0.1f) {
            color=scolor2+dcolor2;
            shadow=sshadow+dshadow;
            offset=0.0f;
            timer2=-1.0f;
        }
        else {
            if(timer2 < delay) { color=scolor2; shadow=sshadow; }
            else { color=scolor2+dcolor2*(timer2-delay)*10; shadow=sshadow+dshadow*(timer2-delay)*10; }
        }
    }
    else if(timer != -1.0f) {
        timer+=dt;
        if(timer >= 0.2f) {
            color=scolor+dcolor;
            offset=soffset+doffset;
            timer=-1.0f;
        }
        else {
            color=scolor+dcolor*timer*5;
            offset=soffset+doffset*timer*5;
        }
    }
    /*Makes the caret blinking effect 
    You might want to change the value below for slower/faster blinking */
    caretTimer += dt; 
    if (caretTimer >= 0.1f){ 
        caretVisible = !caretVisible; 
        caretTimer = 0.0f; 
    } 
}

// This method is called when the GUI
// is about to appear on the screen
void Forminput::Enter() {
    hgeColor tcolor2;

    scolor2.SetHWColor(0x00FF0000);//(0x00FFE060);
    tcolor2.SetHWColor(0xFF7A1494);//(0xFFFFE060);
    dcolor2=tcolor2-scolor2;

    sshadow.SetHWColor(0x00000000);
    tcolor2.SetHWColor(0x30000000);
    dshadow=tcolor2-sshadow;

    timer2=0.0f;
}

// This method is called when the GUI
// is about to disappear from the screen
void Forminput::Leave() {
    hgeColor tcolor2;

    scolor2.SetHWColor(0xFF7A1494);//(0xFFFFE060);
    tcolor2.SetHWColor(0x00FF0000);//(0x00FFE060);
    dcolor2=tcolor2-scolor2;

    sshadow.SetHWColor(0x30000000);
    tcolor2.SetHWColor(0x00000000);
    dshadow=tcolor2-sshadow;

    timer2=0.0f;
}

// This method is called to test whether the control
// have finished it's Enter/Leave animation
bool Forminput::IsDone() {
    if(timer2==-1.0f) return true;
    else return false;
}

// This method is called to notify the control
// that the mouse cursor has entered or left it's area
void Forminput::MouseOver(bool bOver) {
    if(bOver) gui->SetFocus(id);
}

//Gets Focus with LeftMouseKlick 
bool Forminput::MouseLButton(bool bDown){ 
    /*if (bDown){ 
        return false; 
    } 
    gui->SetFocus(id); 
    return false; */
    if(!bDown) {
        offset=4;
        //gui->SetFocus(id); 
        return false;
    }
    else  {
        hge->Effect_PlayEx(snd, EffectVol, 0, 1.0f, false);
        offset=0;
        return false;
    }
}; 

//Saving focus state in focused 
void Forminput::Focus(bool bFocused){ 
    hgeColor tcolor;
    
    if(bFocused) {
        hge->Effect_PlayEx(snd, EffectVol, 0, 1.0f, false);
        scolor.SetHWColor(0xFF7A1494);//(0xFFFFE060);
        tcolor.SetHWColor(0xFFFF0000);//(0xFFFFFFFF);
        soffset=0;
        doffset=4;
        focused = true;
    }
    else {
        scolor.SetHWColor(0xFFFF0000);//(0xFFFFFFFF);
        tcolor.SetHWColor(0xFF7A1494);//(0xFFFFE060);
        soffset=4;
        doffset=-4;
        focused = false;
    }

    dcolor=tcolor-scolor;
    timer=0.0f;
}; 

bool Forminput::KeyClick(int key, int chr){ 
    //Loses Focus with Enter 
    if (key == HGEK_ENTER){ 
        gui->SetFocus(-1); 
        return false; 
    } 
    char tmp[maxchar+1]; 
    strncpy(tmp, content, maxchar+1); 
    if (key == HGEK_BACKSPACE){ //Delete one character per backspace 
        if (caretposition != 0){ 
            caretposition -=1; 
            tmp[caretposition] = '\0'; //Put string terminator one character back 
        } 
        strcpy(content, tmp); 
        length = font->GetStringWidth(content) + 32;
        return false; 
    } 
    /*When chr==0 no ascii-key (e.g. shift) is pressed and chr would 
    be saved as string terminator. The other thing to look after, is 
    whether we have max. characters*/ 
    if (chr != 0 && caretposition < maxchar-1){ 
        tmp[caretposition] = chr; 
        tmp[caretposition+1] = '\0'; //add overwritten string terminator 
        //Assures that the String will not take to much place 
        if (font->GetStringWidth(tmp) <= length){ 
            strcpy(content, tmp); //Store back 
            length = font->GetStringWidth(content) + 32;
            caretposition+=1; 
        } 
    } 
    return false; 
}; 

//Setting the Text 
void Forminput::setText(char *newtext){ 
    strncpy(content ,newtext ,maxchar); 
    content = newtext; 
    length = font->GetStringWidth(content) + 32;
}; 

char *Forminput::getText(){ 
    return content; 
} 
