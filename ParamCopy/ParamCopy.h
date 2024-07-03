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
		// 00 ����t�@�C��
		// 01 �摜�t�@�C��
		// 02 �����t�@�C��
		// 03 �e�L�X�g
		// 04 �}�`
		// 05 �t���[���o�b�t�@
		// 06 �����g�`�\��
		// 07 �V�[��
		// 08 �V�[��(����)
		// 09 ���O�I�u�W�F�N�g
		OT_STD_DRAW = 10,	// �W���`��
		OT_EXT_DRAW = 11,	// �g���`��
		// 12 �W���Đ�
		// 13 �p�[�e�B�N���o��
		// 15 �F���␳
		// 16 �N���b�s���O
		// 17 �΂߃N���b�s���O
		// 18 �ڂ���
		// 21 ���U�C�N
		// 42 �΂߃N���b�v
		// 80 �J�X�^���I�u�W�F�N�g
		OT_PRT_FILT = 88,	// 88 �����t�B���^
		// 93 ���Ԑ���
		OT_GRP_CTRL = 94,	// �O���[�v����
		// 95 �J��������
		// 97 �J��������(��U��)
		// 98 �J��������(�V���h�[)
		// 99 �J��������(�X�N���v�g)
	} ObjectType;

	ObjectType oType ;
	BOOL bFolded;

} ObjectInfo;
