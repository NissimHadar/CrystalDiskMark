﻿/*---------------------------------------------------------------------------*/
//       Author : hiyohiyo
//         Mail : hiyohiyo@crystalmark.info
//          Web : https://crystalmark.info/
//      License : The MIT License
/*---------------------------------------------------------------------------*/

#pragma once

#define WM_THEME_ID			(WM_APP + 0x1600)
#define WM_LANGUAGE_ID		(WM_APP + 0x1800)
#include "DialogCx.h"

class CMainDialog : public CDialogCx
{
public:
	CMainDialog(UINT dlgResouce, 
		CString ThemeDir, DWORD ThemeIndex, CString LangDir, DWORD LangIndex,
		CWnd* pParent = NULL);
	virtual ~CMainDialog();

	CString m_CurrentLocalID;

	CString m_ThemeDir;
	CString m_LangDir;
	DWORD m_ThemeIndex;
	DWORD m_LangIndex;

	CStringArray m_MenuArrayTheme;
	CStringArray m_MenuArrayLang;
	CString m_CurrentTheme;
	CString m_DefaultTheme;
	CString m_RecommendTheme;
	CString m_CurrentLang;

	void InitMenu();
	void InitThemeLang();
	void ChangeTheme(CString ThemeName);
	void SetWindowTitle(CString message);

	BOOL m_FlagStartup;
	BOOL m_FlagInitializing;
	BOOL m_FlagWindoowMinimizeOnce;
	BOOL m_FlagResidentMinimize;

	// Task Tray
	BOOL m_FlagResident;
	static UINT wmTaskbarCreated;
	BOOL AddTaskTray(UINT id, UINT callback, HICON icon, CString tip);
	BOOL RemoveTaskTray(UINT id);
	BOOL ModifyTaskTray(UINT id, HICON icon, CString tip);
	BOOL ModifyTaskTrayIcon(UINT id, HICON icon);
	BOOL ModifyTaskTrayTip(UINT id, CString tip);
	BOOL ShowBalloon(UINT id, DWORD infoFlag, CString infoTitle, CString info);

	DWORD GetZoomType();
	void SetZoomType(DWORD zoomType);

	COLORREF m_LabelText;
	COLORREF m_MeterText;
	COLORREF m_ComboText;
	COLORREF m_ComboTextSelected;
	COLORREF m_ComboBg;
	COLORREF m_ComboBgSelected;
	BYTE     m_ComboAlpha;
	COLORREF m_ButtonText;
	COLORREF m_EditText;
	COLORREF m_EditBg;
	BYTE     m_EditAlpha;
	BYTE     m_CharacterPosition;

	void UpdateThemeInfo();
	COLORREF GetControlColor(CString name, BYTE defaultColor, CString theme);
	BYTE GetControlAlpha(CString name, BYTE defaultAlpha, CString theme);
	BYTE GetCharacterPosition(CString theme);

	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};