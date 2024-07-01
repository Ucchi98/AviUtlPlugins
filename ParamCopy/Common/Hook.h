#pragma once

//--------------------------------------------------------------------
#pragma pack(push)
#pragma pack(1)
//--------------------------------------------------------------------

#define DECLARE_INTERNAL_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName procName

#define IMPLEMENT_INTERNAL_PROC(resultType, callType, procName, args) \
	Type_##procName procName = 0

#define DECLARE_HOOK_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName true_##procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC(resultType, callType, procName, args) \
	Type_##procName true_##procName = procName; \
	resultType callType hook_##procName args

#define IMPLEMENT_HOOK_PROC_NULL(resultType, callType, procName, args) \
	Type_##procName true_##procName = 0; \
	resultType callType hook_##procName args

#define DECLARE_HOOK_PROC(resultType, callType, procName, args) \
	typedef resultType (callType *Type_##procName) args; \
	extern Type_##procName true_##procName; \
	resultType callType hook_##procName args

#define GET_INTERNAL_PROC(module, procName) \
	procName = (Type_##procName)::GetProcAddress(module, #procName)

#define GET_HOOK_PROC(module, procName) \
	true_##procName = (Type_##procName)::GetProcAddress(module, #procName)

#define ATTACH_HOOK_PROC(name) DetourAttach((PVOID*)&true_##name, hook_##name)
#define DETACH_HOOK_PROC(name) DetourDetach((PVOID*)&true_##name, hook_##name)

#define DETACH_HOOK_PROC2(name) \
do \
{ \
	DetourTransactionBegin(); \
	DetourUpdateThread(::GetCurrentThread()); \
 \
	DETACH_HOOK_PROC(name); \
 \
	if (DetourTransactionCommit() == NO_ERROR) \
	{ \
		MY_TRACE(_T(#name) _T(" �̉����ɐ������܂���\n")); \
	} \
	else \
	{ \
		MY_TRACE(_T(#name) _T(" �̉����Ɏ��s���܂���\n")); \
	} \
} \
while (0)

//--------------------------------------------------------------------

// �R�[�h������������B
inline void writeCode(uintptr_t address, const BYTE* code, int c)
{
	// �R�[�h������������B���̂��Ɩ��߃L���b�V�����t���b�V������B
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, code, c, NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, c);
}

// ���� CALL ������������B
template<class T>
inline T hookCall(uintptr_t address, T hookProc)
{
	struct {
		BYTE call;
		uintptr_t func;
	} orig = {
		0xE8, // CALL
	};

	// ���̊֐����擾����B
	::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)address, &orig, sizeof(orig), NULL);

	if (orig.call != 0xE8)
		return 0; // ���� CALL �ł͂Ȃ������B

	struct {
		BYTE call;
		uintptr_t func;
	} code = {
		0xE8, // CALL
		(uintptr_t)hookProc - (address + sizeof(code)),
	};

	// CALL ������������B���̂��Ɩ��߃L���b�V�����t���b�V������B
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));

	// ���̊֐���Ԃ��B
	return (T)(orig.func + (address + sizeof(orig)));
}

// ��� CALL ������������B
template<class T>
inline T hookAbsoluteCall(uintptr_t address, T hookProc)
{
	struct {
		BYTE call[2];
		T* func;
	} orig = {};

	// ���̊֐����擾����B
	::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)address, &orig, sizeof(orig), NULL);

	if (orig.call[0] != 0xFF || orig.call[1] != 0x15)
		return 0; // ��� CALL �ł͂Ȃ������B

	struct {
		BYTE call;
		uintptr_t func;
		BYTE next;
	} code = {
		0xE8, // CALL
		(uintptr_t)hookProc - (address + offsetof(code, next)),
		0x90, // NOP
	};

	// CALL ������������B���̂��Ɩ��߃L���b�V�����t���b�V������B
	::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &code, sizeof(code), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(code));

	// ���̊֐���Ԃ��B
	return *orig.func;
}

// ��΃A�h���X������������B
template<class T>
inline T writeAbsoluteAddress(uintptr_t address, T x)
{
	// ���̒l���擾����B
	T retValue = 0;
	BOOL result1 = ::ReadProcessMemory(::GetCurrentProcess(), (LPVOID)address, &retValue, sizeof(retValue), NULL);

	// ��΃A�h���X������������B���̂��Ɩ��߃L���b�V�����t���b�V������B
	BOOL result2 = ::WriteProcessMemory(::GetCurrentProcess(), (LPVOID)address, &x, sizeof(x), NULL);
	::FlushInstructionCache(::GetCurrentProcess(), (LPVOID)address, sizeof(x));

	// ���̒l��Ԃ��B
	return retValue;
}

// �A�h���X���L���X�g����B
template<class T>
inline void castAddress(T& x, uintptr_t address)
{
	x = (T)address;
}

//--------------------------------------------------------------------

template<class T>
T hookImportFunc(HMODULE module, LPCSTR funcName, T func)
{
	struct Hooker
	{
		LPCSTR m_funcName;
		T m_newFunc;
		T m_oldFunc;

		static BOOL CALLBACK detour(
			_In_opt_ PVOID pContext,
			_In_ DWORD nOrdinal,
			_In_opt_ LPCSTR pszFunc,
			_In_opt_ PVOID* ppvFunc)
		{
			Hooker* hooker = (Hooker*)pContext;

			MY_TRACE_STR(pszFunc);

			if (!pszFunc) return TRUE;
			if (::lstrcmpA(pszFunc, hooker->m_funcName) != 0) return TRUE;

			DWORD protect = PAGE_READWRITE;
			BOOL result1 = ::VirtualProtect(ppvFunc, sizeof(*ppvFunc), protect, &protect);
			hooker->m_oldFunc = writeAbsoluteAddress((DWORD)ppvFunc, hooker->m_newFunc);
			BOOL result2 = ::VirtualProtect(ppvFunc, sizeof(*ppvFunc), protect, &protect);

			return FALSE;
		}

	} hooker = { funcName, func };

	DetourEnumerateImportsEx(module, &hooker, 0, hooker.detour);

	return hooker.m_oldFunc;
}

//--------------------------------------------------------------------

template<class T>
T rewriteFunction(DWORD base, LPCSTR funcName, T func)
{
	// IMAGE_IMPORT_DESCRIPTOR ���擾����B
	PIMAGE_DOS_HEADER dos = (PIMAGE_DOS_HEADER)base;
	PIMAGE_NT_HEADERS nt = (PIMAGE_NT_HEADERS)((DWORD)dos + dos->e_lfanew);
	PIMAGE_IMPORT_DESCRIPTOR importDesc = (PIMAGE_IMPORT_DESCRIPTOR)
		((DWORD)dos + nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	for (; importDesc->Name; importDesc++)
	{
		// IMAGE_THUNK_DATA ���擾����B
		PIMAGE_THUNK_DATA firstThunk = (PIMAGE_THUNK_DATA)(base + importDesc->FirstThunk);
		PIMAGE_THUNK_DATA originalFirstThunk = (PIMAGE_THUNK_DATA)(base + importDesc->OriginalFirstThunk);

		for (;firstThunk->u1.Function; firstThunk++, originalFirstThunk++)
		{
			// �I�[�f�B�i�����`�F�b�N����B
			if (IMAGE_SNAP_BY_ORDINAL(originalFirstThunk->u1.Ordinal)) continue;

			// �֐��̖��O���擾����B
			PIMAGE_IMPORT_BY_NAME importName = (PIMAGE_IMPORT_BY_NAME)(base + originalFirstThunk->u1.AddressOfData);

			// �֐��̖��O���`�F�b�N����B
			if (::lstrcmpA((LPCSTR)importName->Name, funcName) != 0) continue;

			// ���̊֐����擾����B
			T retValue = 0;
			::ReadProcessMemory(::GetCurrentProcess(), &firstThunk->u1.Function, &retValue, sizeof(T), NULL);

			// �֐�������������B
			::WriteProcessMemory(::GetCurrentProcess(), &firstThunk->u1.Function, &func, sizeof(T), NULL);
		}
	}

	return NULL;
}

//--------------------------------------------------------------------
#pragma pack(pop)
//--------------------------------------------------------------------
