#include <Windows.h>
#include <detours.h>
#pragma comment (lib, "detours.lib")
 
typedef BOOL(__stdcall * f_wglSwapBuffers) (_In_ HDC hDc);
f_wglSwapBuffers original_wglSwapBuffers;
 
DWORD frames = 0xDEADBEEF; // delay 
 
BOOL _stdcall hooked_wglSwapBuffers(_In_ HDC hDc) {
    Sleep(frames);
    return original_wglSwapBuffers(hDc);
}
 
DWORD WINAPI Main_Thread(LPVOID lpParam) {
 
    HMODULE hMod = GetModuleHandleW(L"opengl32.dll");
    if (!hMod)
        return S_OK;
    original_wglSwapBuffers = (f_wglSwapBuffers)(DWORD)GetProcAddress(hMod, "wglSwapBuffers");
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)original_wglSwapBuffers, hooked_wglSwapBuffers);
    DetourTransactionCommit();
   
    MessageBoxA(0, "DLL injected successfully,\nFPS limited", "Success", MB_OK | MB_APPLMODAL);
 
    return S_OK;
}
 
BOOL APIENTRY DllMain(HMODULE hModule, DWORD _reason, LPVOID lpReserved) {
 
    if (_reason == DLL_PROCESS_ATTACH) {
        CreateThread(0, 0x1000, &Main_Thread, 0, 0, NULL);
    }
    return TRUE;
}
