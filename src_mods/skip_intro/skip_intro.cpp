#include <windows.h>

#include "../game_profile.hpp"

std::uintptr_t titleSceneRva;

extern "C" __declspec(naked) void StartTitleScene() {
    __asm__ volatile(
        "mov _gameBase, %eax\n"
        "mov _titleSceneRva, %ecx\n"
        "add %ecx, %eax\n"
        "call *%eax\n"
        "ret\n"
    );
}

extern "C" void InitializeSkipIntro() {
    if (!GetPrivateProfileIntA("Intro", "Skip", 1, ".\\crazy_loto_tweaks.ini")) {
        return;
    }
    titleSceneRva = gameProfile->titleScene;
    const auto call = gameBase + gameProfile->skipIntroCall;
    const auto replacement = reinterpret_cast<std::uintptr_t>(&StartTitleScene);
    const auto relative = static_cast<std::int32_t>(replacement - call - 5);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(call + 1), &relative, 4, nullptr);
}
