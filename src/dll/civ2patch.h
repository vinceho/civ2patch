#ifndef CIV2PATCH_H
#define CIV2PATCH_H

#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_DLL
#define CIV2PATCH_API __declspec(dllexport)
#else
#define CIV2PATCH_API __declspec(dllimport)
#endif

BOOL CIV2PATCH_API WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpvReserved);
BOOL CIV2PATCH_API PeekMessageEx(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg);
MCIERROR CIV2PATCH_API mciSendCommandEx(MCIDEVICEID IDDevice, UINT uMsg, DWORD_PTR fdwCommand, DWORD_PTR dwParam);

#ifdef __cplusplus
}
#endif

#endif // CIV2PATCH_H
