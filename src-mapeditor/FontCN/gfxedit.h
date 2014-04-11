//////////////////////////////////////////////////////////////////////////
//
// 编辑输入框类
//
// 作者：微妙的平衡(BOGY)
//
// 版本：For HGE v1.7
//
//////////////////////////////////////////////////////////////////////////
#ifndef GDIEDIT_H
#define GDIEDIT_H

#include <hgerect.h>
#include	"GfxFont.h"
#pragma   warning(disable:4312)
#pragma   warning(disable:4311)

class GfxEdit
{
public:

	///
	/// nEditWidth		编辑框宽度
	///	nFontColor		字体颜色
	/// lpFontName		字体名称
	///	nFontSize		字体大小
	/// bBold			是否粗体
	/// bItalic			是否斜体
	/// bAntialias		是否平滑
	///
	GfxEdit(UINT nEditWidth, DWORD nFontColor, CHAR* lpFontName = "宋体", UINT nFontSize = 12, BOOL bBold = FALSE, BOOL bItalic = FALSE, BOOL bAntialias = TRUE);
	~GfxEdit(void);

	typedef enum tagDeletePos
	{
		DP_LEFT = 0,
		DP_RIGHT
	}TDP;

public:
	void			SetFocus(); // 设置编辑框焦点，只有拥有编辑框焦点才能响应输入
	void			KillFocus(); // 清除编辑框焦点，失去编辑框焦点后将不再响应输入

	void			ClearCookie(); // 清除编辑框内容

	void			InsertCookie(const char* lpCookie); // 插入字符串到编辑框当前光标处
	const char*		GetCookie(); // 获取编辑框当前内容

	void			InsertCookieW(const wchar_t* lpCookie); // 插入字符串到编辑框当前光标处(宽字符模式)
	const wchar_t*	GetCookieW(); // 获取编辑框当前内容(宽字符模式)

	void			Updata(float dt);
	void			Render(float x, float y); //
	void			ShowOuterFrame(bool	show)		{IsShowOuterFrame=show;}
	bool			GetOuterFrame()	const			{return IsShowOuterFrame;}
	void			SetNumOnly(bool	NumOnly)		{IsNumOnly=NumOnly;}
	bool			DoKeyDown();
	BOOL			CopyStringToClipboard(wchar_t* WText);
	BOOL			SetStrToClipboard();
	BOOL			GetStrFromClipboard();
	BOOL			SetCuttingClip();

private:
	void			OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	void			OnCharHZ( UINT nChar, UINT nRepCnt, UINT nFlags );
	void			InsertChar(wchar_t aChar);
	void			DeleteChar(TDP aPos);
	void			DeleteMoreChar();
	void			MouseLButton();
	void			MouseLMove();
	void			SelectAllChar();

private:
	static const int COOKIE_SIZE = 1024;
	wchar_t			m_sCookie[COOKIE_SIZE];
	char			m_sBuffer[COOKIE_SIZE];

	FLOAT			m_nEditW;
	FLOAT			m_nEditH;

	UINT			m_nCharPos;		// 字符位置

	hgeRect			m_rcWorld;
    
	HGE*			m_pHGE;			// HGE指针
	hgeSprite*		m_pSprite;		// 光标精灵
	hgeSprite*		bgSprite;		// 光标选字精灵
	GfxFont*		m_pFont;		// 字体精灵

	bool			IsShowOuterFrame;	//外边框
	bool			IsShowCursor;		//光标
	bool			IsNumOnly;			//纯数字
	bool			IsCuttingClipOn;	//剪切板

	bool			MouseLDown;
	UINT			MouseL1CharPos;
	float			MouseL1x,MouseL1y;
	UINT			MouseL2CharPos;
	float			MouseL2x,MouseL2y;
	float			LastDtTime;

	float			box_x;
	float			box_y;
	float			box_w;
	float			box_h;

	static LRESULT CALLBACK GfxEditWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
};
#endif//GDIEDIT_H
