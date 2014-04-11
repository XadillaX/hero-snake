
#include "GfxEdit.h"
#include <atlbase.h>


WNDPROC		g_lpLastHgeWndProc = NULL;
GfxEdit*	g_lpFocusEditPtr = NULL;

LRESULT	CALLBACK GfxEdit::GfxEditWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_KEYDOWN == nMsg || WM_KEYUP == nMsg)
	{
		MSG aMsg;
		memset(&aMsg,0,sizeof(MSG));

		aMsg.hwnd	= hWnd;
		aMsg.message= nMsg;
		aMsg.wParam	= wParam;
		aMsg.lParam	= lParam;
		aMsg.time	= GetTickCount();

		TranslateMessage(&aMsg);

		if (g_lpFocusEditPtr && WM_KEYDOWN == nMsg)
		{
			switch(wParam)
			{
			case VK_DELETE: //key delete
			case VK_LEFT: //key left
			case VK_RIGHT: //key right
			case VK_HOME: //key home
			case VK_END: //key end
				g_lpFocusEditPtr->OnChar(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
				break;

			}
		}
	}
	else if (WM_CHAR == nMsg )
	{
		if (g_lpFocusEditPtr)
		{
			g_lpFocusEditPtr->OnChar(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}
	}
	else if (WM_IME_CHAR == nMsg)
	{
		if (g_lpFocusEditPtr)
		{
			g_lpFocusEditPtr->OnCharHZ(static_cast<UINT>(wParam), LOWORD(lParam), HIWORD(lParam));
			return TRUE;
		}
	}

	return g_lpLastHgeWndProc?g_lpLastHgeWndProc(hWnd,nMsg,wParam,lParam):TRUE;
}


GfxEdit::GfxEdit(UINT nEditWidth, DWORD nFontColor, CHAR* lpFontName, UINT nFontSize, BOOL bBold, BOOL bItalic, BOOL bAntialias)
{
	m_pHGE = hgeCreate(HGE_VERSION);
	if (!m_pHGE) throw NULL;

	HWND hWnd = m_pHGE->System_GetState(HGE_HWND);
	if (!hWnd) throw NULL;

	if (g_lpLastHgeWndProc == NULL)
	{
		g_lpLastHgeWndProc = (WNDPROC)::GetWindowLong(hWnd,GWL_WNDPROC);
		if(g_lpLastHgeWndProc != (WNDPROC)::SetWindowLong(hWnd,GWL_WNDPROC,(LONG)GfxEditWndProc)) throw NULL;
	}

	nEditWidth = (nEditWidth == 0 ? 1 : nEditWidth);
	nFontSize = (nFontSize == 0 ? 12 : nFontSize);

	m_nEditW = static_cast<FLOAT>(nEditWidth);
	m_nEditH = static_cast<FLOAT>(nFontSize);

	m_pSprite = new hgeSprite(NULL,0,0,1.2f*float(nFontSize)/12,m_nEditH);
	m_pSprite->SetColor(nFontColor);

	bgSprite = new hgeSprite(NULL,0,0,1.2f*float(nFontSize)/12,m_nEditH);
	bgSprite->SetColor(0x99BCCEF7);

	m_pFont = new GfxFont(lpFontName,nFontSize,bBold,bItalic,bAntialias);
	if (!m_pFont) throw NULL;
	m_pFont->SetColor(nFontColor);

	m_nCharPos = 0;

	m_rcWorld.Set(0,0,0,0);

	ClearCookie();
	IsCuttingClipOn = false;
	IsShowOuterFrame = true;
	IsShowCursor = false;
	IsNumOnly = false;
	MouseLDown = false;
	LastDtTime = 0;
	box_x = 0;
	box_y = 0;
	box_w = (float)nEditWidth;
	box_h = m_nEditH;
	MouseL1CharPos = MouseL2CharPos = 0;
	MouseL1x = MouseL1y = MouseL2x = MouseL2y = 0;
}

GfxEdit::~GfxEdit(void)
{
	if (m_pSprite)	delete	m_pSprite;
	if (bgSprite)	delete	bgSprite;
	if (m_pFont)	delete	m_pFont;
	if (m_pHGE)		m_pHGE->Release();
}

void GfxEdit::SetFocus()
{
	g_lpFocusEditPtr = this;
}

void GfxEdit::KillFocus()
{
	g_lpFocusEditPtr = NULL;
}

void GfxEdit::ClearCookie()
{
	memset(m_sCookie,0,sizeof(m_sCookie));
	m_nCharPos = 0;
}

void GfxEdit::InsertCookie(const char* lp)
{
	InsertCookieW(CA2W(lp));
}

const char* GfxEdit::GetCookie()
{
	strcpy_s(m_sBuffer,CW2A(m_sCookie));
	return m_sBuffer;
}

void GfxEdit::InsertCookieW(const wchar_t* lp)
{
	if (lp)
	{
		while(*lp)
		{
			InsertChar(*lp++);
		}
	}
}

const wchar_t* GfxEdit::GetCookieW()
{
	return m_sCookie;
}

void GfxEdit::Updata(float dt)
{
	float mouse_x = 0;
	float mouse_y = 0;
	m_pHGE->Input_GetMousePos(&mouse_x,&mouse_y);

	if (m_pHGE->Input_GetKeyState(HGEK_LBUTTON))
	{
		if (m_rcWorld.TestPoint(mouse_x,mouse_y))
		{
			SetFocus();
		}
	}
	LastDtTime+=dt;
	if(LastDtTime>=0.5f)
	{
		IsShowCursor=true;
		if(LastDtTime>=1.0f)	LastDtTime=0;
	}
	else
	{
		IsShowCursor=false;
	}

	//鼠标

	if(	m_pHGE->Input_KeyDown(HGEK_LBUTTON)	)
	{
		if(mouse_x>box_x && mouse_x<(box_x + box_w) && mouse_y>box_y && mouse_y<(box_y + box_h))	MouseLButton();
		else	KillFocus();
	}			

	if(MouseLDown)
	{
		if(	m_pHGE->Input_KeyUp(HGEK_LBUTTON))	MouseLDown=false;
		else	MouseLMove();
	}
}

void GfxEdit::Render(float x, float y)
{
	box_x = x - 1;
	box_y = y - 1;
	box_w = m_nEditW + 2;
	box_h = m_nEditH + 2;

	if(IsShowOuterFrame)
	{
		// 外框
		m_pHGE->Gfx_RenderLine(box_x, box_y, box_x + box_w + 1, box_y);
		m_pHGE->Gfx_RenderLine(box_x, box_y, box_x, box_y + box_h + 1);
		m_pHGE->Gfx_RenderLine(box_x + box_w, box_y + box_h, box_x + box_w, box_y);
		m_pHGE->Gfx_RenderLine(box_x + box_w, box_y + box_h, box_x, box_y + box_h);
	}

	// 保存编辑框区域
	m_rcWorld.Set(x,y,x+m_nEditW,y+m_nEditH);

	float text_x = x;
	float text_y = y;
	float char_x = x;
	float char_y = y;

	if (m_nCharPos > 0)
	{
		wchar_t sTmp[COOKIE_SIZE];
		memset(sTmp,0,sizeof(sTmp));
		wcsncpy_s(sTmp,m_sCookie,m_nCharPos);
		SIZE szTmp = m_pFont->GetTextSize(sTmp);
		float tmp_w = static_cast<float>(szTmp.cx);

		if (tmp_w > m_nEditW)
		{
			text_x -= (tmp_w - m_nEditW);
		}

		char_x = text_x + tmp_w;
	}

	int clip_x = static_cast<int>(box_x);
	int clip_y = static_cast<int>(box_y);
	int clip_w = static_cast<int>(box_w);
	int clip_h = static_cast<int>(box_h);

	m_pHGE->Gfx_SetClipping(clip_x, clip_y, clip_w, clip_h);

	if (g_lpFocusEditPtr == this )  bgSprite->RenderStretch(MouseL1x+1, MouseL1y, MouseL2x+1, MouseL2y);
	m_pFont->Render(text_x,text_y,m_sCookie);
	if (g_lpFocusEditPtr == this && IsShowCursor==true) m_pSprite->Render(char_x-1,char_y);

	m_pHGE->Gfx_SetClipping();
}

void GfxEdit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(DoKeyDown())
	{}
	else if (m_nCharPos < COOKIE_SIZE)
	{
		static   int   i=0;
		static char szImeChar[3];
		szImeChar[2]='\0';

		if(nChar&0x80)
		{   
				if(i!=2)
				{   
					szImeChar[i]=(char)nChar; 
					i++;//纪下次字符值   
				}
				
				if(i==2)
				{
					i=0;
					InsertChar(*CA2W(szImeChar));
				}
         }
		 else   
         {   
				i=0;                           //正常处理   
				szImeChar[0]=(char)nChar;   
				szImeChar[1]='\0'; 
				InsertChar(*CA2W(szImeChar));
          }   
	}

	LastDtTime=0.5f;		//光标显示

	//当有键盘输入操作时，取肖光标选字状态
	if( m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_A))	//CTRL+A，全选
	{}
	else
	{
		MouseLDown=false;
		MouseL1CharPos=MouseL2CharPos=m_nCharPos;
		MouseL1x = MouseL1y = MouseL2x = MouseL2y = 0;
	}
}

void GfxEdit::OnCharHZ( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (m_nCharPos < COOKIE_SIZE)
	{
		char szImeChar[3];

		if (nChar < 128)
		{
			szImeChar[0]=(char)nChar;   
			szImeChar[1]='\0';   
		}
		else
		{
			szImeChar[0]=(char)(nChar>>8);   
			szImeChar[1]=(char)nChar;
			szImeChar[2]='\0';
		}

		InsertChar(*CA2W(szImeChar));
	}

	LastDtTime=0.5f;		//光标显示

	//当有键盘输入操作时，取肖光标选字状态
	MouseLDown=false;
	MouseL1CharPos=MouseL2CharPos=m_nCharPos;
	MouseL1x = MouseL1y = MouseL2x = MouseL2y = 0;

}

void GfxEdit::InsertChar(wchar_t aChar)
{
	if(MouseL1CharPos != MouseL2CharPos)	DeleteMoreChar();
	UINT nLen = (UINT)wcslen(m_sCookie);

	if(IsNumOnly)
	{
		if( aChar ==L'0' || aChar ==L'1' || aChar ==L'2' || aChar ==L'3' || aChar ==L'4' || 
		aChar ==L'5' || aChar ==L'6' || aChar ==L'7' || aChar ==L'8'  || aChar ==L'9'|| aChar ==L'.' )
		{
			if (m_nCharPos < nLen)
			{
				while(m_nCharPos < nLen)
				{
					m_sCookie[nLen] = m_sCookie[nLen-1];
					--nLen;
				}
			}
			m_sCookie[m_nCharPos++] = aChar;
		}
	}
	else
	{
		if (m_nCharPos < nLen)
		{
			while(m_nCharPos < nLen)
			{
				m_sCookie[nLen] = m_sCookie[nLen-1];
				--nLen;
			}
		}
		m_sCookie[m_nCharPos++] = aChar;
	}
}

void GfxEdit::DeleteChar(TDP aPos)
{
	if (DP_LEFT == aPos)
	{
		if (m_nCharPos == 0)
		{
			return;
		}
		--m_nCharPos;
	}
	else if (DP_RIGHT == aPos)
	{
		if (m_nCharPos == (UINT)wcslen(m_sCookie))
		{
			return;
		}
	}
	else
	{
		return;
	}

	UINT nIndex = m_nCharPos;
	while(m_sCookie[nIndex] != 0)
	{
		m_sCookie[nIndex] = m_sCookie[nIndex+1];
		nIndex++;
	}
}


void GfxEdit::MouseLButton()
{
	float	text_x = box_x;
	float	PosX,PosY;
	float	FontX=0;
	float	tmpFontX=0;
	int		nNum;
	SIZE	FontsW = m_pFont->GetTextSize(m_sCookie);
	float	charW  = m_pFont->GetWidthFromCharacter(m_sCookie[0]);
	m_pHGE->Input_GetMousePos(&PosX,&PosY);

	if( PosX<(box_x+FontsW.cx) )
	{
		nNum=m_nCharPos;
		m_nCharPos=0;

		wchar_t sTmp[COOKIE_SIZE];
		memset(sTmp,0,sizeof(sTmp));
		wcsncpy_s(sTmp,m_sCookie,nNum);
		SIZE szTmp = m_pFont->GetTextSize(sTmp);
		float tmp_w = static_cast<float>(szTmp.cx);

		//文字长度超出文本框宽度时光标定位的补位
		if (tmp_w > m_nEditW)
		{
			text_x = (tmp_w - m_nEditW);
			while(tmpFontX < text_x)
			{
				charW=m_pFont->GetWidthFromCharacter(m_sCookie[m_nCharPos]);
				tmpFontX+=charW;
				m_nCharPos++;
			}
		}

		while((box_x + charW/2 +FontX)<PosX)
		{
			charW=m_pFont->GetWidthFromCharacter(m_sCookie[m_nCharPos]);
			FontX+=charW;
			m_nCharPos++;
		}
	}
	else
	{
		m_nCharPos = (UINT)wcslen(m_sCookie);
	}

	LastDtTime=0.5f;		//光标显示
	MouseLDown=true;
	MouseL1CharPos=MouseL2CharPos=m_nCharPos;
	MouseL1x = MouseL1y = MouseL2x = MouseL2y = 0;
}

bool GfxEdit::DoKeyDown()
{
	if (m_pHGE->Input_GetKeyState(HGEK_TAB) && !IsNumOnly) // key tab
	{
		for (int i=0;i<4;++i) InsertChar(L' ');	// insert 7 space
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_BACKSPACE)) // key back space
	{
		if(MouseL1CharPos != MouseL2CharPos)	DeleteMoreChar();
		else	DeleteChar(DP_LEFT);
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_DELETE)) // key delete
	{
		DeleteChar(DP_RIGHT);
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_HOME))	//HOME
	{
		m_nCharPos = 0;
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_END))	//END
	{
		m_nCharPos = (UINT)wcslen(m_sCookie);
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_LEFT)) // key left
	{
		if (m_nCharPos > 0)
		{
			--m_nCharPos;
		}
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_RIGHT)) // key right
	{
		if (m_nCharPos < wcslen(m_sCookie))
		{
			++m_nCharPos;
		}
		return	true;
	}
	else if (m_pHGE->Input_GetKeyState(HGEK_ESCAPE)) // key ESC
	{
		return	true;
	}
	else if(m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_Z))	//CTRL+Z，反悔
	{
		return	true;
	}
	else if(m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_C))	//CTRL+C，复制
	{
		SetStrToClipboard();
		return	true;
	}
	else if(m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_V))	//CTRL+V，粘帖
	{
		GetStrFromClipboard();
		return	true;
	}
	else if(m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_X))	//CTRL+X，剪切
	{
		SetCuttingClip();
		return	true;
	}
	else if(m_pHGE->Input_GetKeyState(HGEK_CTRL) && m_pHGE->Input_GetKeyState(HGEK_A))	//CTRL+A，全选
	{
		SelectAllChar();
		return	true;
	}



	return	false;
}

void GfxEdit::MouseLMove()
{
	float	PosX,PosY;
	float	charW = 0;
	UINT	TmpCharPos;
	wchar_t sTmp[COOKIE_SIZE];
	memset(sTmp,0,sizeof(sTmp));
	wcsncpy_s(sTmp,m_sCookie,MouseL1CharPos);
	SIZE szTmp = m_pFont->GetTextSize(sTmp);
	float tmp_w = static_cast<float>(szTmp.cx);

	float text_x = box_x;
	if (tmp_w > m_nEditW)
	{
		text_x -= (tmp_w - m_nEditW);
	}

	MouseL1x = text_x + tmp_w + 1 - 2;	//补正坐标（请参考GfxEdit::Render(float x, float y)）
	if(MouseL2x==0)		MouseL2x = MouseL1x;

	if(MouseL2CharPos > (UINT)wcslen(m_sCookie)) MouseL2CharPos=(UINT)wcslen(m_sCookie);
	if(MouseL2CharPos < 0)	MouseL2CharPos=0;
	m_pHGE->Input_GetMousePos(&PosX,&PosY);
	if(PosX > MouseL2x)
	{
		TmpCharPos=MouseL2CharPos;
		if(TmpCharPos==(UINT)wcslen(m_sCookie))		TmpCharPos--;
		charW=m_pFont->GetWidthFromCharacter(m_sCookie[++TmpCharPos]);
		if(	(MouseL2x + charW/2) <  PosX)	
		{
			if(MouseL2CharPos < (UINT)wcslen(m_sCookie))
			{
				MouseL2CharPos++;
				m_nCharPos++;
			}
		}
	}
	else if(PosX < MouseL2x)
	{
		TmpCharPos=MouseL2CharPos;
		if(TmpCharPos==0)	TmpCharPos++;
		charW=m_pFont->GetWidthFromCharacter(m_sCookie[--TmpCharPos]);
		if(	(MouseL2x - charW/2) >  PosX)	
		{
			if(MouseL2CharPos > 0)	
			{
				MouseL2CharPos--;
				m_nCharPos--;
			}
		}
	}

	wchar_t sTmp2[COOKIE_SIZE];
	memset(sTmp2,0,sizeof(sTmp2));
	wcsncpy_s(sTmp2,m_sCookie,MouseL2CharPos);
	SIZE szTmp2 = m_pFont->GetTextSize(sTmp2);
	float tmp_w2 = static_cast<float>(szTmp2.cx);

	text_x = box_x;
	if (tmp_w2 > m_nEditW)
	{
		text_x -= (tmp_w2 - m_nEditW);
	}

	MouseL2x = text_x + tmp_w2 + 1 - 2;	//补正坐标（请参考GfxEdit::Render(float x, float y)）
	MouseL1y = box_y;
	MouseL2y = box_y + box_h;

}


void GfxEdit::DeleteMoreChar()
{
	if(MouseL1CharPos<MouseL2CharPos)
	{
		for(int i=(int)MouseL1CharPos; i < (int)MouseL2CharPos; i++)	DeleteChar(DP_LEFT);
	}
	else if(MouseL1CharPos>MouseL2CharPos)
	{
		for(int i=(int)MouseL1CharPos; i > (int)MouseL2CharPos; i--)	DeleteChar(DP_RIGHT);
	}

	MouseL1CharPos=MouseL2CharPos=m_nCharPos;
}

BOOL GfxEdit::CopyStringToClipboard(wchar_t* WText)
{
	 int nlen = (int)wcslen(WText);
	 if (nlen == 0)	return FALSE;

	 HGLOBAL	hGlobal;
	 wchar_t*	pGlobal;

	 if (!::OpenClipboard(NULL))	return FALSE;

	 hGlobal = GlobalAlloc(GMEM_DDESHARE,(nlen + 1) * sizeof(wchar_t));
	 
	 if (hGlobal == NULL)
	 {
		  CloseClipboard();
		  return FALSE;
	 }

	 EmptyClipboard();

	 pGlobal = (wchar_t*)GlobalLock(hGlobal);
	 wcscpy_s(pGlobal,nlen + 1, WText);
	 GlobalUnlock(hGlobal);
	  
	 SetClipboardData(CF_UNICODETEXT, hGlobal);
	 CloseClipboard();
	 return TRUE;
}


BOOL GfxEdit::SetStrToClipboard()    
{  
	int i=0;
	int	j=0;
	wchar_t   buffer[COOKIE_SIZE]; 
	memset(buffer,0,sizeof(buffer));

	if(MouseL1CharPos == MouseL2CharPos)	return FALSE;

	if(MouseL1CharPos<MouseL2CharPos)
	{
		for(i=(int)MouseL1CharPos; i < (int)MouseL2CharPos; i++)	
		{
			buffer[j]=m_sCookie[i];
			j++;
		}
		wcsncpy_s(buffer,buffer,MouseL2CharPos-MouseL1CharPos);
	}
	else if(MouseL1CharPos>MouseL2CharPos)
	{
		for(i=(int)MouseL2CharPos; i < (int)MouseL1CharPos; i++)
		{
			buffer[j]=m_sCookie[i];
			j++;
		}
		wcsncpy_s(buffer,buffer,MouseL1CharPos-MouseL2CharPos);
	}

	CopyStringToClipboard(buffer);
	return TRUE;
}   



BOOL GfxEdit::GetStrFromClipboard()    
{  
	PTSTR   pGlobal;  
	HGLOBAL   hGlobal;  
	wchar_t   buffer[1024];  
	if (!::OpenClipboard(NULL))	return FALSE;

	if   (hGlobal   =   GetClipboardData(CF_UNICODETEXT))  
	{  
		pGlobal   =   (TCHAR*)GlobalLock(hGlobal);   
		wcscpy_s(buffer,(wchar_t*)pGlobal); 
		GlobalUnlock(hGlobal);
	}
	else
	{
		CloseClipboard();
		return FALSE;
	}
	
	if(MouseL1CharPos != MouseL2CharPos)	DeleteMoreChar();
	InsertCookieW(buffer);
	if(IsCuttingClipOn)	
	{
		IsCuttingClipOn = false;
		EmptyClipboard();
		CloseClipboard(); 
		return FALSE;
	}
	CloseClipboard(); 
	return TRUE;
 }   

BOOL GfxEdit::SetCuttingClip()
{  
	if(MouseL1CharPos == MouseL2CharPos) return FALSE;
	SetStrToClipboard();
	DeleteMoreChar();
	IsCuttingClipOn = true;
	return TRUE;
 }   


void GfxEdit::SelectAllChar()
{  
	MouseL1CharPos = 0;
	MouseL2CharPos = m_nCharPos = (UINT)wcslen(m_sCookie);
	SIZE szTmp = m_pFont->GetTextSize(m_sCookie);
	float tmp_w = static_cast<float>(szTmp.cx);
	MouseL1x = box_x + 1 - 2;;
	MouseL2x = box_x + tmp_w + 1 - 2;	//补正坐标（请参考GfxEdit::Render(float x, float y)）
	MouseL1y = box_y;
	MouseL2y = box_y + box_h;
}