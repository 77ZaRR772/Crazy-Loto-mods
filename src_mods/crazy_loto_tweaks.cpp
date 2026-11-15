#include <windows.h>

#include "game_profile.hpp"

extern "C" void InitializeNoShake();
extern "C" void InitializeSnakeSpeed();
extern "C" void InitializeDurakControls();
extern "C" void InitializeGunControls();
extern "C" void InitializeFishTime();
extern "C" void InitializeSkipIntro();
extern "C" void InitializeTextSettings();

extern "C" __declspec(dllexport) void InitializeASI() {
    if (!DetectGameVersion()) {
        return;
    }
    InitializeSkipIntro();
    InitializeNoShake();
    InitializeSnakeSpeed();
    InitializeDurakControls();
    InitializeGunControls();
    InitializeFishTime();
    InitializeTextSettings();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) {
    return TRUE;
}
