﻿/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#include "stdafx.h"
#include "DiskMark.h"
#include "ComboBoxCx.h"
#include "FontComboBox.h"


// CFontComboBox

IMPLEMENT_DYNAMIC(CFontComboBox, CComboBox)

CFontComboBox::CFontComboBox()
{
	m_FontHeight = (LONG)(-1 * 16 * 1.00);
}

CFontComboBox::~CFontComboBox()
{
}


BEGIN_MESSAGE_MAP(CFontComboBox, CComboBoxCx)
END_MESSAGE_MAP()

void CFontComboBox::SetFontHeight(int height, double zoomRatio)
{
	m_FontHeight = (LONG)(-1 * height * zoomRatio);
}

void CFontComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = abs(m_FontHeight);
}

void CFontComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_bHighContrast)
	{
		m_TextColor = RGB(255, 255, 255);
		m_BgColor = RGB(0, 0, 0);
		m_TextSelectedColor = RGB(0, 0, 0);
		m_BgSelectedColor = RGB(0, 255, 255);
	}

    CString cstr;
    if (lpDrawItemStruct->itemID == -1)
        return;
    GetLBText(lpDrawItemStruct->itemID, cstr);
    CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

    CFont font;
    LOGFONT logfont;
    memset(&logfont, 0, sizeof(logfont));
    logfont.lfHeight = m_FontHeight;
    logfont.lfWidth = 0;
    logfont.lfWeight = 400;
	logfont.lfQuality = 6;
	logfont.lfCharSet = DEFAULT_CHARSET;
    pDC->SelectObject(&font);
	if (cstr.GetLength() < 32)
	{
	  _tcscpy_s(logfont.lfFaceName, 32, (LPCTSTR)cstr);
	}
    font.CreateFontIndirect(&logfont);
    pDC->SelectObject(&font);

	CBrush Brush;
	CBrush* pOldBrush;

	if (lpDrawItemStruct->itemState & ODS_SELECTED) {
		Brush.CreateSolidBrush(m_BgSelectedColor);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextSelectedColor);
	}
	else {
		Brush.CreateSolidBrush(m_BgColor);
		pOldBrush = pDC->SelectObject(&Brush);
		FillRect(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, (HBRUSH)Brush);
		SetTextColor(lpDrawItemStruct->hDC, m_TextColor);
	}
	pDC->SelectObject(pOldBrush);
	Brush.DeleteObject();

	pDC->SetBkMode(TRANSPARENT);

	lpDrawItemStruct->rcItem.left = (LONG)(4 * m_ZoomRatio);

    pDC->DrawText(cstr, &lpDrawItemStruct->rcItem, DT_SINGLELINE | DT_VCENTER);
}


int CFontComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct)
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