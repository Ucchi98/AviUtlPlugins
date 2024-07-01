#include <windows.h>
#include <windowsX.h>
#include <winuser.h>
#include <tchar.h>
#include <strsafe.h>

#include "filter.h"
#include "ParamCopy.h"

//---------------------------------------------------------------------
//		フィルタ構造体定義
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_NO_CONFIG,
	0, 0,
	(LPSTR)"ParamCopy",
	NULL, NULL, NULL,
	NULL, NULL,
	NULL, NULL, NULL,
	NULL,
	func_init,
	func_exit,
	NULL,
	NULL,
	NULL, NULL,
	NULL,
	NULL,
	(LPSTR)"ParamCopy version 1.00 by Ucchi",
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL
};

//---------------------------------------------------------------------
//		初期化
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp)
{
	BOOL bRes = InitParamCopy();
	return bRes;
}

//---------------------------------------------------------------------
//		終了
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp)
{
	return TRUE;
}

//---------------------------------------------------------------------
//		フィルタ構造体のポインタを渡す関数
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &filter;
}
