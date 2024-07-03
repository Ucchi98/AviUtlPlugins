#include <windows.h>
#include <windowsX.h>
#include <winuser.h>
#include <tchar.h>
#include <strsafe.h>

#include "filter.h"
#include "ParamCopy.h"

//---------------------------------------------------------------------
//		�t�B���^�\���̒�`
//---------------------------------------------------------------------
FILTER_DLL filter = {
	FILTER_FLAG_ALWAYS_ACTIVE |
	FILTER_FLAG_NO_CONFIG,
	0, 0,
	(LPSTR)FILTER_INFO,
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
	(LPSTR)FILTER_INFO,
	NULL, NULL,
	NULL, NULL, NULL, NULL,
	NULL
};

//---------------------------------------------------------------------
//		������
//---------------------------------------------------------------------
BOOL func_init(FILTER* fp)
{
	BOOL bRes = InitParamCopy();
	return bRes;
}

//---------------------------------------------------------------------
//		�I��
//---------------------------------------------------------------------
BOOL func_exit(FILTER* fp)
{
	return TRUE;
}

//---------------------------------------------------------------------
//		�t�B���^�\���̂̃|�C���^��n���֐�
//---------------------------------------------------------------------
EXTERN_C FILTER_DLL __declspec(dllexport)* __stdcall GetFilterTable(void)
{
	return &filter;
}
