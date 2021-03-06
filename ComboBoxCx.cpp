﻿//---------------------------------------------------------------------------
// CComboBoxCx Copyright (C) 2019 Crystal Dew World All rights reserved.
//---------------------------------------------------------------------------
// CAlphaButton をベースにカスタマイズ
//---------------------------------------------------------------------------
// CAlphaButton Copyright(C)2007 ソフト屋 巣 All rights reserved.
//---------------------------------------------------------------------------
// 特徴:
// ・ホバー状態をサポートしています。
// ・ボタンの５状態を画像で表示することが出来ます。
// ・アルファ付の画像をサポートしています。GDI+を使用。
// ・ツールチップをサポートしています。
//---------------------------------------------------------------------------
// ライセンス(修正BSDライセンスを元にしています)
//---------------------------------------------------------------------------
// CAlphaButton Copyright(C)2007 ソフト屋 巣 All rights reserved.
// 1.ソース・コード形式であれバイナリ形式であれ、変更の有無にかかわらず、以下の条件を満たす限りにおいて、再配布および使用を許可します
// 1-1.ソース・コード形式で再配布する場合、上記著作権表示、本条件書および第2項の責任限定規定を必ず含めてください。
// 1-2.バイナリ(DLLやライブラリ等)形式で再配布する場合、上記著作権表示、本条件書および下記責任限定規定を、配布物とともに提供される文書および／または他の資料に必ず含めてください。
// 1-3.アプリケーションに組み込んで配布する場合は特に著作権表示は必要ありませんが、下記責任限定規定を、配布物とともに提供される文書および／または他の資料に含めてください。
// 2.本ソフトウェアは無保証です。自己責任で使用してください。
// 3.著作権者の名前を、広告や宣伝に勝手に使用しないでください。
//---------------------------------------------------------------------------
// 更新履歴
//---------------------------------------------------------------------------
// Ver0.802 2007/12/06 アルファの無い画像のバグを修正。
// Ver0.801 2007/12/05 リソースのロードに対応しました。
// 	pngもリソース化できます。
// Ver0.800 2007/12/05 GDI+を使わないコードに変更してみました。
// 	遅いようなら、AlphaButton.hのUSE_GDIPを変更すればGDI+に戻せます。
// Ver0.701 2007/11/26 GDI+を使ったバージョンで公開。
//---------------------------------------------------------------------------

#include "stdafx.h"
#include "ComboBoxCx.h"
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// 計測スイッチ
#define	CHECK_DRAWTIME	(0)	// ボタンの描画時間を計測する場合は1にする。

BOOL CComboBoxCx::IsHighContrast()
{
	HIGHCONTRAST hc;
	hc.cbSize = sizeof(HIGHCONTRAST);
	SystemParametersInfo(SPI_GETHIGHCONTRAST, sizeof(HIGHCONTRAST), &hc, 0);
	return hc.dwFlags & HCF_HIGHCONTRASTON;
}

CComboBoxCx::CComboBoxCx()
	: m_bHover(FALSE)
	, m_bFocas(FALSE)
	, m_bTrackingNow(FALSE)
	, m_bSelected(FALSE)
	, m_GpFont(NULL)
	, m_GpBrush(NULL)
	, m_GpBrushSelected(NULL)
	, m_GpStringformat(NULL)
	, m_TextAlign(BS_LEFT)
	, m_bHandCursor(FALSE)
	, m_RenderMode(0)
	, m_bHighContrast(FALSE)
	, m_FontType(FT_GDI)
	, m_TextColor(0)
	, m_ZoomRatio(1.0)
	, m_FontHeight(16)
{
	m_Margin.top = 0;
	m_Margin.left = 0;
	m_Margin.bottom = 0;
	m_Margin.right = 0;

	m_TextColor = RGB(0, 0, 0);
	m_TextSelectedColor = RGB(0, 0, 0);
	m_BgColor = RGB(255, 255, 255);
	m_BgSelectedColor = RGB(230, 230, 230);
}

CComboBoxCx::~CComboBoxCx()
{
	m_Font.DeleteObject();
	SAFE_DELETE(m_GpFont);
	SAFE_DELETE(m_GpBrush);
	SAFE_DELETE(m_GpStringformat);
}

IMPLEMENT_DYNAMIC(CComboBoxCx, CComboBox)

BEGIN_MESSAGE_MAP(CComboBoxCx, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxCx)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

//------------------------------------------------
// ツールチップ関連
//------------------------------------------------

// ツールチップを初期化する。
void CComboBoxCx::InitToolTip()
{
	// ツールチップは未初期化？
	if (m_ToolTip.m_hWnd == NULL)
	{
		// ツールチップを初期化する。
		m_ToolTip.Create(this, TTS_ALWAYSTIP | TTS_BALLOON | TTS_NOANIMATE | TTS_NOFADE);
		// ツールチップを無効にする。
		m_ToolTip.Activate(FALSE);
		// ツールチップにフォントを設定します。
		m_ToolTip.SetFont(&m_Font);
		// ツールチップを複数行対応にする。
		m_ToolTip.SendMessageW(TTM_SETMAXTIPWIDTH, 0, 1024);
		// ツールチップを表示するまでの時間を設定する。
		// m_ToolTip.SetDelayTime(1000);
	}
}

// コントロールに渡る前のメッセージを処理。
BOOL CComboBoxCx::PreTranslateMessage(MSG* pMsg)
{
	// ツールチップを初期化する。
	InitToolTip();

	// ツール ヒント コントロールにメッセージを渡して処理。
	m_ToolTip.RelayEvent(pMsg);

	return CComboBox::PreTranslateMessage(pMsg);
}

CString CComboBoxCx::GetToolTipText()
{
	return m_ToolTipText;
}

// ツールチップのメッセージを設定する。
void CComboBoxCx::SetToolTipText(LPCTSTR pText)
{
	// ツールチップを初期化する。
	InitToolTip();

	// テキストが有効？
	if (pText == NULL) { return; }

	CString toolTipText = pText;
	// テキストの内容が異なる？
	// if (!toolTipText.IsEmpty() && m_ToolTipText.Find(toolTipText) == 0) { return; }
	m_ToolTipText = toolTipText;

	// ツール ヒント コントロールに登録されているツールの数は無効？
	if (TRUE /*m_ToolTip.GetToolCount() == 0*/)
	{
		// クライアント領域のサイズを取得します
		CRect rect;
		GetClientRect(rect);
		// ツール ヒント コントロールにツールを登録します
		if (m_ToolTip.GetToolCount() == 1)
		{
			m_ToolTip.DelTool(this);
		}
		m_ToolTip.AddTool(this, m_ToolTipText, rect, 1);
	}
	else
	{
		// ツールにツール ヒント テキストを設定します。
		m_ToolTip.UpdateTipText(m_ToolTipText, this, 1);
	}
	// ツールチップを有効にします。
	SetToolTipActivate(TRUE);
}

// ツールチップの有効無効を切り替えます。
void CComboBoxCx::SetToolTipActivate(BOOL bActivate)
{
	// ツール ヒント コントロールに登録されているツールの数は無効？
	if (m_ToolTip.GetToolCount() == 0) return;

	// ツールチップの有効無効を切り替えます。
	m_ToolTip.Activate(bActivate);
}

// コントロールとツールチップのメッセージを設定する。
void CComboBoxCx::SetToolTipWindowText(LPCTSTR pText)
{
	SetToolTipText(pText);
	SetWindowText(pText);
}

void CComboBoxCx::SetFontHeight(int height, double zoomRatio)
{
	m_FontHeight = (LONG)(-1 * height * zoomRatio);
	m_ZoomRatio = zoomRatio;
}

void CComboBoxCx::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = abs(m_FontHeight);
}

void CComboBoxCx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_bHighContrast)
	{
		m_TextColor = RGB(255, 255, 255);
		m_BgColor = RGB(0, 0, 0);
		m_TextSelectedColor = RGB(0, 0, 0);
		m_BgSelectedColor = RGB(0, 255, 255);
	}
	
	CString cstr = L"";
	if (lpDrawItemStruct->itemID == -1)
		return;
	GetLBText(lpDrawItemStruct->itemID, cstr);
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	CBrush Brush;
	CBrush* pOldBrush;

	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		Brush.CreateSolidBrush(m_BgSelectedColor);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextSelectedColor);
	}
	else
	{
		Brush.CreateSolidBrush(m_BgColor);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextColor);
	}
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();

	pDC->SetBkMode(TRANSPARENT);

	if (m_bHighContrast)
	{
		pDC->DrawText(cstr, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
	}
	else
	{
		lpDrawItemStruct->rcItem.left = (LONG)(4 * m_ZoomRatio);
		DrawString(cstr, pDC, lpDrawItemStruct);
	}
}


void CComboBoxCx::DrawString(CString title, CDC* drawDC, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// テキストが空の場合および GDI+ Font/Brush が設定されていない場合何もしない。
	if (title.IsEmpty() || m_GpFont == NULL || m_GpBrush == NULL || m_GpBrushSelected == NULL)
	{
		return;
	}

	// 透過モードにする。
	drawDC->SetBkMode(TRANSPARENT);

	// テキストの描画位置
	CRect rect = (CRect)(lpDrawItemStruct->rcItem); // クライアント四角形の取得
	// マージン設定
	rect.top += m_Margin.top;
	rect.left += m_Margin.left;
	rect.bottom -= m_Margin.bottom;
	rect.right -= m_Margin.right;

	CArray<CString, CString> arr;
	arr.RemoveAll();

	CString resToken;
	int curPos = 0;
	resToken = title.Tokenize(L"\r\n", curPos);
	while (resToken != L"")
	{
		arr.Add(resToken);
		resToken = title.Tokenize(L"\r\n", curPos);
	}

	if (m_FontType >= FT_GDI_PLUS_1) // GDI+
	{
		Gdiplus::Graphics g(drawDC->m_hDC);

		const Gdiplus::PointF pointF(0.0, 0.0);
		Gdiplus::RectF extentF;

		for (int i = 0; i < arr.GetCount(); i++)
		{
			CRect r;
			r.top = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * i);
			r.bottom = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * (i + 1));
			r.left = rect.left;
			r.right = rect.right;

			g.MeasureString(arr.GetAt(i), arr.GetAt(i).GetLength() + 1, m_GpFont, pointF, &extentF); // "+ 1" for workdaround 

			REAL y;
			FontFamily ff;
			m_GpFont->GetFamily(&ff);
			REAL ascent = (REAL)ff.GetCellAscent(FontStyleRegular);
			REAL descent = (REAL)ff.GetCellDescent(FontStyleRegular);
			REAL lineSpacing = (REAL)ff.GetLineSpacing(FontStyleRegular);

			switch (m_FontType)
			{
			case FT_GDI_PLUS_2:
				y = rect.CenterPoint().y - (extentF.Height * (ascent) / lineSpacing) / 2;
				break;
			case FT_GDI_PLUS_3:
				y = rect.CenterPoint().y - (extentF.Height * (ascent) / (ascent + descent)) / 2;
				break;
			default:
				y = rect.CenterPoint().y - (extentF.Height * (ascent + descent) / lineSpacing) / 2;
				break;
			}

			Gdiplus::PointF pt((REAL)rect.left, y);
			Gdiplus::RectF rectF(pt.X, pt.Y, (REAL)extentF.Width, (REAL)extentF.Height);

			g.SetTextRenderingHint(TextRenderingHintAntiAlias);
			if (lpDrawItemStruct->itemState & ODS_SELECTED)
			{
				g.DrawString(arr.GetAt(i), -1, m_GpFont, rectF, m_GpStringformat, m_GpBrushSelected);
			}
			else
			{
				g.DrawString(arr.GetAt(i), -1, m_GpFont, rectF, m_GpStringformat, m_GpBrush);
			}
		}
	}
	else // GDI
	{
		for (int i = 0; i < arr.GetCount(); i++)
		{
			CRect r;
			r.top = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * i);
			r.bottom = rect.top + (LONG)(((double)rect.Height()) / arr.GetCount() * (i + 1));
			r.left = rect.left;
			r.right = rect.right;

			CRect rectI;
			CSize extent;
			HGDIOBJ oldFont = drawDC->SelectObject(m_Font);
			// SetTextColor(drawDC->m_hDC, m_TextColor);
			GetTextExtentPoint32(drawDC->m_hDC, arr.GetAt(i), arr.GetAt(i).GetLength() + 1, &extent);
			rectI.top = r.top + (r.Height() - extent.cy) / 2;
			rectI.bottom = rectI.top + extent.cy;
			rectI.left = r.left;
			rectI.right = r.right;
			DrawText(drawDC->m_hDC, arr.GetAt(i), arr.GetAt(i).GetLength(), r, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
			drawDC->SelectObject(oldFont);
		}
	}
	// いつか DirectWrite 描画に対応したいものである。。。
}

//------------------------------------------------
// マウスの移動関連
//------------------------------------------------

// マウスの移動
void CComboBoxCx::OnMouseMove(UINT nFlags, CPoint point)
{
	// トラッキングしていない？
	if (!m_bTrackingNow)
	{
		// トラッキングをチェック。
		TRACKMOUSEEVENT tme;
		tme.cbSize = sizeof(tme);
		tme.hwndTrack = m_hWnd;
		tme.dwFlags = TME_LEAVE | TME_HOVER;
		tme.dwHoverTime = 1;
		m_bTrackingNow = _TrackMouseEvent(&tme);
	}
	CComboBox::OnMouseMove(nFlags, point);
}

// マウスが上にあるイベント。
void CComboBoxCx::OnMouseHover(UINT nFlags, CPoint point)
{
	CComboBox::OnMouseHover(nFlags, point);

	// ホバー有効
	m_bHover = TRUE;
	// 再描画
	Invalidate();
}

// マウスが上から離れた場合のイベント。
void CComboBoxCx::OnMouseLeave()
{
	CComboBox::OnMouseLeave();

	// トラッキングが外れた。
	m_bTrackingNow = FALSE;
	// ホバー無効
	m_bHover = FALSE;
	// 再描画
	Invalidate();
}

// フォーカスが有効
void CComboBoxCx::OnSetfocus()
{
	// フォーカス有効
	m_bFocas = TRUE;
	// 再描画
	Invalidate();
}

// フォーカスが無効
void CComboBoxCx::OnKillfocus()
{
	// フォーカス無効
	m_bFocas = FALSE;
	// 再描画
	Invalidate();
}

BOOL CComboBoxCx::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (m_bHandCursor)
	{
		// ハンドカーソルに変更
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	}
	else
	{
		// 通常カーソルに戻す
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	return TRUE;
}

void CComboBoxCx::SetHandCursor(BOOL bHandCuror)
{
	m_bHandCursor = bHandCuror;
}

//------------------------------------------------
// フォント関連
//------------------------------------------------
/*
void CComboBoxCx::SetFontEx(CString face, int size, double zoomRatio)
{
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio);
	logFont.lfQuality = 6;
	if (face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
	SetFont(&m_Font);

	if (m_ToolTip.m_hWnd != NULL)
	{
		m_ToolTip.SetFont(&m_Font);
	}

	SetItemHeight(-1, (UINT)(size * zoomRatio * 1.25));
}
*/

void CComboBoxCx::SetFontEx(CString face, int size, double zoomRatio, BYTE textAlpha, COLORREF textColor, COLORREF textSelectedColor, LONG fontWeight, INT fontType)
{
	if (m_bHighContrast)
	{
		m_TextColor = RGB(255, 255, 255);
		m_TextSelectedColor = RGB(0, 0, 0);
	}
	else
	{
		m_TextColor = textColor;
		m_TextSelectedColor = textSelectedColor;
	}

	m_ZoomRatio = zoomRatio;
	LOGFONT logFont = { 0 };
	logFont.lfCharSet = DEFAULT_CHARSET;
	logFont.lfHeight = (LONG)(-1 * size * zoomRatio);
	logFont.lfQuality = 6;
	logFont.lfWeight = fontWeight;
	if (face.GetLength() < 32)
	{
		wsprintf(logFont.lfFaceName, _T("%s"), face.GetString());
	}
	else
	{
		wsprintf(logFont.lfFaceName, _T(""));
	}

	m_Font.DeleteObject();
	m_Font.CreateFontIndirect(&logFont);
	SetFont(&m_Font);


	// フォント描画方法を設定します。
	if (FT_AUTO <= fontType && fontType <= FT_GDI_PLUS_3)
	{
		m_FontType = fontType;
	}
	else
	{
		m_FontType = FT_AUTO;
	}

	// ツールチップにフォントを設定します。
	if (m_ToolTip.m_hWnd != NULL)
	{
		m_ToolTip.SetFont(&m_Font);
	}

	CDC* pDC = GetDC();
	SAFE_DELETE(m_GpFont);
	SAFE_DELETE(m_GpBrushSelected);
	SAFE_DELETE(m_GpStringformat);
	m_GpFont = new Gdiplus::Font(pDC->m_hDC, m_Font);
	m_GpBrush = new Gdiplus::SolidBrush(
		Gdiplus::Color(textAlpha, GetRValue(textColor), GetGValue(textColor), GetBValue(textColor)));
	m_GpBrushSelected = new Gdiplus::SolidBrush(
		Gdiplus::Color(textAlpha, GetRValue(textSelectedColor), GetGValue(textSelectedColor), GetBValue(textSelectedColor)));

	m_GpStringformat = new Gdiplus::StringFormat;
	m_GpStringformat->SetAlignment(StringAlignmentCenter);
	m_GpStringformat->SetLineAlignment(StringAlignmentCenter);
	m_GpStringformat->SetFormatFlags(StringFormatFlagsNoClip);
	ReleaseDC(pDC);

	SetItemHeight(-1, (UINT)(size * zoomRatio * 1.25));
}

int CComboBoxCx::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	/*
	LPCTSTR lpszText1 = (LPCTSTR) lpCompareItemStruct->itemData1;
	ASSERT(lpszText1 != NULL);
	LPCTSTR lpszText2 = (LPCTSTR) lpCompareItemStruct->itemData2;
	ASSERT(lpszText2 != NULL);

	return _tcscmp(lpszText2, lpszText1);
	*/
	return -1;
}

HBRUSH CComboBoxCx::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	switch (nCtlColor) {
	case CTLCOLOR_EDIT:
		pDC->SetBkMode(TRANSPARENT);
		return hbr;
	default:
		return hbr;
	}
}

void CComboBoxCx::SetMargin(int top, int left, int bottom, int right, double zoomRatio)
{
	m_Margin.top = (int)(top * zoomRatio);
	m_Margin.left = (int)(left * zoomRatio);
	m_Margin.bottom = (int)(bottom * zoomRatio);
	m_Margin.right = (int)(right * zoomRatio);
}

BOOL CComboBoxCx::InitControl(int x, int y, int width, int height, double zoomRatio, UINT renderMode)
{
	MoveWindow((int)(x * zoomRatio), (int)(y * zoomRatio), (int)(width * zoomRatio), (int)(height * zoomRatio));

	m_bHighContrast = FALSE;

	if (renderMode & HighContrast)
	{
		m_bHighContrast = TRUE;
	}
	
	return TRUE;
}

void CComboBoxCx::SetBgColor(COLORREF bgColor, COLORREF bgSelectedColor)
{
	m_BgColor = bgColor;
	m_BgSelectedColor = bgSelectedColor;
}