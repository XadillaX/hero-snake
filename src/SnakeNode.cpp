#include "SnakeNode.h"

SnakeNode::SnakeNode(char* FileName, float rx, float ry, float w, float h, int x, int y,HGE* hge):x(x), y(y), hge(hge) {
    tex = hge->Texture_Load(FileName);
    body = new hgeSprite(tex, rx, ry, w, h);
}

void SnakeNode::Render(DWORD c, bool ishead, bool DR) {
    if (ishead)
    {
        if(!DR) body->SetColor(0xFFFF0099);
        else body->SetColor(0xff84C1D7);
    }
    else
        body->SetColor(c);
    body->Render(80 + 32 * x, 60 + 32 * y);
}

SnakeNode::~SnakeNode(void) {
    delete body;
    hge->Texture_Free(tex);
}
