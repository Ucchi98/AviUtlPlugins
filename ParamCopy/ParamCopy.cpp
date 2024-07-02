#include <windows.h>
#include <windowsX.h>
#include <winuser.h>
#include <tchar.h>
#include <strsafe.h>

#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#include "detours.h"
#pragma comment(lib, ".\\Detours\\lib.X86\\detours.lib")

#include "resource.h"

#include "aviutl.hpp"
#include "exedit.hpp"
#include "Tracer.h"
#include "Hook.h"
#include "AviUtlInternal.h"

#include "ParamCopy.h"

#define BUTTON_ORG_X			(214)
#define BUTTON_ORG_W	        (60)
#define BUTTON_ORG_H	        (22)
#define BUTTON_NEW_X			(229)
#define BUTTON_NEW_W	        (30)
#define BUTTON_NEW_H	        (22)
#define BUTTON_PARAMCOPY_W		(14)
#define BUTTON_PARAMCOPY_H		(22)

static AviUtlInternal aui;
static ParamCopyRow wp[8] = { 0 };
static HWND hWndBkup[8] = { 0 };
static bool is_fold = false;

BOOL InitParamCopy()
{
	aui.initExEditAddress();

	// Hook Window Procedure of Setting Dialog
	true_SettingDialogProc = aui.HookSettingDialogProc(hook_SettingDialogProc);

	// Hook Windows API
	DetourTransactionBegin();
	DetourUpdateThread(::GetCurrentThread());

	ATTACH_HOOK_PROC(SetWindowPos);

	if (DetourTransactionCommit() == NO_ERROR)
	{
		MY_TRACE(_T("API フックに成功しました\n"));
		return true;
	}
	else
	{
		MY_TRACE(_T("API フックに失敗しました\n"));
		return false;
	}
}

HWND CreateButton(HWND hParentWnd, int nButtonID, int x, int y, HANDLE hIcon)
{
	// Create Button
	HWND hWndBn = CreateWindowEx(
		WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_STATICEDGE | WS_EX_RIGHTSCROLLBAR, // Styles 
		_T("Button"),                                                            // Predefined class; Unicode assumed 
		_T(""),                                                                  // Button text 
		WS_VISIBLE | WS_CHILDWINDOW | BS_PUSHBUTTON | BS_ICON,                   // Styles
		x,                                                                       // x position 
		y,                                                                       // y position 
		BUTTON_PARAMCOPY_W,                                                      // Button width
		BUTTON_PARAMCOPY_H,                                                      // Button height
		hParentWnd,                                                              // Parent window
		(HMENU)nButtonID,                                                        // Button Id
		(HINSTANCE)GetWindowLongPtr(hParentWnd, GWLP_HINSTANCE),
		NULL);                                                                   // Pointer not needed.

	// Set Icon
	SendMessage(hWndBn, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

	return hWndBn;
}

void AdjustButtonPos(HWND hWnd, int *nX, int *nY, int *nW, int *nH, UINT *uFlags)
{
	// X よりも上にあるフォームは処理しない
	if (*nY < 44) return;

	// X、Y、Z、拡大率、透明度、回転ボタンと編集ボックスのウインドウハンドルを保存
	char sClassName[32];
	int nRes = GetClassName(hWnd, sClassName, 32);
	if (nRes == 0) return;

	int nvIdx = 0;
	int nvIdxMax = 6;
	if (strncmp(sClassName, "Button", 6) == 0)
	{
		if (((*nY - 46) % 25) != 0) return;

		nvIdx = (*nY - 46) / 25;
		if (nvIdx < nvIdxMax)
		{
			if (*nX == 214) wp[nvIdx].hWndCBN = hWnd;
		}
	}

	if (strncmp(sClassName, "Edit", 4) == 0)
	{
		if (((*nY - 49) % 25) != 0) return;

		nvIdx = (*nY - 49) / 25;
		if (nvIdx < nvIdxMax)
		{
			if (*nX == 168) wp[nvIdx].hWndLED = hWnd;
			if (*nX == 274) wp[nvIdx].hWndRED = hWnd;
		}
	}
}

void CopyParam(HWND hWnd, int nIdBn)
{
	int nIdxBn = nIdBn - BN_PARAMCOPY_START;
	int nIdxRow = nIdxBn >> 1;

	HWND hWndSrc;
	HWND hWndDst;

	if (nIdxBn % 2 == 0) {
		// Begin to End
		hWndSrc = wp[nIdxRow].hWndLED;
		hWndDst = wp[nIdxRow].hWndRED;
	} else {
		// End to Begin
		hWndSrc = wp[nIdxRow].hWndRED;
		hWndDst = wp[nIdxRow].hWndLED;
	}

	char sBuf[32];
	GetWindowText(hWndSrc, sBuf, 16);
	SetFocus(hWndDst);
	SetWindowText(hWndDst, sBuf);
	SetFocus(hWnd);
}

ObjectType GetCurrentObject()
{
	// 選択しているオブジェクトに ParamCopy ボタンを追加する対象のフィルタが登録されているか確認
	ObjectType otCurrent = OT_NONE;
	int nObjIdx = aui.GetCurrentObjectIndex();
	if (nObjIdx >= 0) {
		ExEdit::Object* o = aui.GetObject(nObjIdx);
		if (o != NULL) {
			ExEdit::Object::FilterParam* fp = o->filter_param;
			ExEdit::Object::FilterStatus* fs = o->filter_status;
			for (int i = 0; i < 12; i++) {
				if (fp[i].is_valid()) {
					if (fp[i].id == OT_STD_DRAW || fp[i].id == OT_EXT_DRAW || fp[i].id == OT_GRP_CTRL) {
						otCurrent = (ObjectType)fp[i].id;
						is_fold = (((unsigned char)fs[0] & (unsigned char)ExEdit::Object::FilterStatus::Folding) == (unsigned char)ExEdit::Object::FilterStatus::Folding);
						{
							//ExEdit::Filter* f = aui.GetFilter(fp[i].id);
							//MY_TRACE("DispName:%s,Idx:%02d,Id:%02d,Flag: %s %s %s %s %s %s %s %s, FilterName: %s\n",
							//	o->dispname,
							//	i,
							//	fp[i].id,
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::Input ? "Input" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::Output ? "Output" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::Effect ? "Effect" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::HasExdata ? "HasExdata" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::BasicEffect ? "BasicEffect" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::Audio ? "Audio" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::Control ? "Control" : ""),
							//	((uint32_t)f->flag & (uint32_t)ExEdit::Filter::Flag::ExEditFilter ? "ExEditFilter" : ""),
							//	f->name
							//);
						}
						break;
					}
				}
			}
		}
	}

	MY_TRACE("GetCurrentObject: Id: %02d, Fold: %s\n", otCurrent, is_fold ? "Folded" : "Normal");
	return otCurrent;
}

IMPLEMENT_HOOK_PROC_NULL(LRESULT, WINAPI, SettingDialogProc, (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam))
{
	switch (message)
	{
	case WM_CREATE:
		MY_TRACE(_T("WM_CREATE\n"));
		{
			// Create buttons for ParamCopy.
			HINSTANCE hInst = GetModuleHandle(_T("ParamCopy.auf"));
			HANDLE hIconLA = LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_LA), IMAGE_ICON, 16, 16, 0);
			HANDLE hIconRA = LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON_RA), IMAGE_ICON, 16, 16, 0);

			wp[0].hWndLBN = CreateButton(hwnd, ID_BN_X_B, 214,  46, hIconLA);  // X Begin
			wp[0].hWndRBN = CreateButton(hwnd, ID_BN_X_E, 259,  46, hIconRA);  // X End
			wp[1].hWndLBN = CreateButton(hwnd, ID_BN_Y_B, 214,  71, hIconLA);  // Y Begin
			wp[1].hWndRBN = CreateButton(hwnd, ID_BN_Y_E, 259,  71, hIconRA);  // Y End
			wp[2].hWndLBN = CreateButton(hwnd, ID_BN_Z_B, 214,  96, hIconLA);  // Z Begin
			wp[2].hWndRBN = CreateButton(hwnd, ID_BN_Z_E, 259,  96, hIconRA);  // Z End
			wp[3].hWndLBN = CreateButton(hwnd, ID_BN_M_B, 214, 121, hIconLA);  // ZoomRatio Begin
			wp[3].hWndRBN = CreateButton(hwnd, ID_BN_M_E, 259, 121, hIconRA);  // ZoomRatio End
			wp[4].hWndLBN = CreateButton(hwnd, ID_BN_T_B, 214, 146, hIconLA);  // Transparency Begin
			wp[4].hWndRBN = CreateButton(hwnd, ID_BN_T_E, 259, 146, hIconRA);  // Transparency End
			wp[5].hWndLBN = CreateButton(hwnd, ID_BN_R_B, 214, 171, hIconLA);  // Rotation Begin
			wp[5].hWndRBN = CreateButton(hwnd, ID_BN_R_E, 259, 171, hIconRA);  // Rotation End
		}
		break;

	case WM_SHOWWINDOW:
		MY_TRACE(_T("WM_SHOWWINDOW\n"));
		{
			ObjectType otCurrent = GetCurrentObject();

			// ボタンサイズ変更によりボタンの文字を短く変更
			int nvIdxMax = 0;
			int nCmdShow = (is_fold ? SW_HIDE : SW_SHOW);
			switch (otCurrent)
			{
			case OT_STD_DRAW:
				SetWindowText(wp[3].hWndCBN, _T("拡大"));
				SetWindowText(wp[4].hWndCBN, _T("透明"));
				nvIdxMax = 6;
				break;
			case OT_EXT_DRAW:
				SetWindowText(wp[3].hWndCBN, _T("拡大"));
				SetWindowText(wp[4].hWndCBN, _T("透明"));
				nvIdxMax = 5;
				break;
			case OT_GRP_CTRL:
				SetWindowText(wp[3].hWndCBN, _T("拡大"));
				nvIdxMax = 4;
				break;
			default:
				nvIdxMax = 0;
				nCmdShow = SW_HIDE;
				break;
			}

			// Restore Button Size
			for (int nIdx = 0; nIdx < 6; nIdx++)
			{
				if (hWndBkup[nIdx] != NULL)
				{
					true_SetWindowPos(hWndBkup[nIdx], NULL, 0, 0, BUTTON_ORG_W, BUTTON_ORG_H, SWP_NOZORDER | SWP_NOMOVE);
				}
			}

			// Show/Hide ParamCopy Button and Set Button Pos and Size
			for (int nIdx = 0; nIdx < nvIdxMax; nIdx++)
			{
				ShowWindow(wp[nIdx].hWndLBN, nCmdShow);
				ShowWindow(wp[nIdx].hWndRBN, nCmdShow);

				true_SetWindowPos(wp[nIdx].hWndCBN, NULL, BUTTON_NEW_X, nIdx * 25 + 46, BUTTON_NEW_W, BUTTON_NEW_H, SWP_NOZORDER);

				// サイズを変更したボタンは元に戻す必要があるので記録しておく
				hWndBkup[nIdx] = wp[nIdx].hWndCBN;
			}
			for (int nIdx = nvIdxMax; nIdx < 6; nIdx++)
			{
				ShowWindow(wp[nIdx].hWndLBN, SW_HIDE);
				ShowWindow(wp[nIdx].hWndRBN, SW_HIDE);

				hWndBkup[nIdx] = NULL;
			}
		}
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_BN_X_B:	case ID_BN_X_E:
		case ID_BN_Y_B:	case ID_BN_Y_E:
		case ID_BN_Z_B:	case ID_BN_Z_E:
		case ID_BN_M_B:	case ID_BN_M_E:
		case ID_BN_T_B:	case ID_BN_T_E:
		case ID_BN_R_B:	case ID_BN_R_E:
			CopyParam(hwnd, wParam);
			break;
		}
		break;

	default:
		break;
	}
	return true_SettingDialogProc(hwnd, message, wParam, lParam);
}

IMPLEMENT_HOOK_PROC(BOOL, WINAPI, SetWindowPos, (HWND hWnd, HWND hWndInsertAfter, int x, int y, int w, int h, UINT uFlags))
{
	MY_TRACE("SetWinPos,HWND:0x%08X,X:%02d,Y:%02d,W:%02d,H:%02d,F:0x%08X\n", hWnd, x, y, w, h, uFlags);

	AdjustButtonPos(hWnd, &x, &y, &w, &h, &uFlags);
	return true_SetWindowPos(hWnd, hWndInsertAfter, x, y, w, h, uFlags);
}

void ___outputLog(LPCTSTR text, LPCTSTR output)
{
	::OutputDebugString(output);
}
