#pragma once

#include "Tracer.h"
#include "Hook.h"

DECLARE_HOOK_PROC(BOOL, WINAPI, SetWindowPos, (HWND hWnd, HWND hWndInsertAfter, int x, int y, int cx, int cy, UINT uFlags));
DECLARE_HOOK_PROC(BOOL, WINAPI, SetWindowText, (HWND hWnd, LPCSTR lpString));
DECLARE_HOOK_PROC(LRESULT, WINAPI, SettingDialogProc, (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam));

extern BOOL InitParamCopy();

#define FILTER_NAME "ParamCopy"
#define FILTER_VER  "1.3.0"
#define FILTER_BY   "by Ucchi"
#define FILTER_INFO "" FILTER_NAME """ Ver.""" FILTER_VER """ """ FILTER_BY ""

#define ID_BN_PARAMCOPY_START 0x7890

typedef enum : UINT16 {
	ID_BN_X_B = ID_BN_PARAMCOPY_START,
	ID_BN_X_E,

	ID_BN_Y_B,
	ID_BN_Y_E,

	ID_BN_Z_B,
	ID_BN_Z_E,

	ID_BN_M_B,
	ID_BN_M_E,

	ID_BN_T_B,
	ID_BN_T_E,

	ID_BN_R_B,
	ID_BN_R_E
} ParamCopyButton;

typedef struct {
	HWND hWndLED;	// left edit
	HWND hWndLBN;	// left button for param copy
	HWND hWndCBN;	// center button
	HWND hWndRBN;	// right button for param copy
	HWND hWndRED;	// right edit
	LONG wCBNID;    // center button id
} ParamCopyRow;

typedef enum {
	CA_BN   = 0xc018, // Button
	CA_EDIT = 0xc019, // Edit
	CA_STAT = 0xc01a, // Static
	CA_CB   = 0xc01d, // ComboBox
	CA_TB   = 0xc16a, // TrackBar
	CA_UD   = 0xc16c, // UpDown
} ClassAtom;

typedef struct {
	typedef enum {
		OT_NONE = -1,
		// 00 動画ファイル
		// 01 画像ファイル
		// 02 音声ファイル
		// 03 テキスト
		// 04 図形
		// 05 フレームバッファ
		// 06 音声波形表示
		// 07 シーン
		// 08 シーン(音声)
		// 09 直前オブジェクト
		OT_STD_DRAW = 10,	// 標準描画
		OT_EXT_DRAW = 11,	// 拡張描画
		// 12 標準再生
		// 13 パーティクル出力
		// 15 色調補正
		// 16 クリッピング
		// 17 斜めクリッピング
		// 18 ぼかし
		// 21 モザイク
		// 42 斜めクリップ
		// 80 カスタムオブジェクト
		OT_PRT_FILT = 88,	// 88 部分フィルタ
		// 93 時間制御
		OT_GRP_CTRL = 94,	// グループ制御
		// 95 カメラ制御
		// 97 カメラ効果(手振れ)
		// 98 カメラ制御(シャドー)
		// 99 カメラ制御(スクリプト)
	} ObjectType;

	ObjectType oType ;
	BOOL bFolded;

} ObjectInfo;
