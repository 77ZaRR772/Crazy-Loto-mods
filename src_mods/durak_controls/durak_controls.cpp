#include <windows.h>

#include "../game_profile.hpp"

int durakSpaceEnabled = 1;
bool durakToggled = false;
std::uintptr_t durakKeyFunctionRva;

extern "C" bool DurakToggleState() {
    if (durakSpaceEnabled && (GetAsyncKeyState(VK_SPACE) & 1)) {
        durakToggled = !durakToggled;
    }
    return durakToggled;
}

extern "C" __declspec(naked) void DurakKeyDown() {
    __asm__ volatile(
        "push %ecx\n"
        "mov _gameBase, %ecx\n"
        "add _durakKeyFunctionRva, %ecx\n"
        "call *%ecx\n"
        "test %al, %al\n"
        "jnz 1f\n"
        "call _DurakToggleState\n"
        "1:\n"
        "pop %ecx\n"
        "ret\n"
    );
}

extern "C" void InitializeDurakControls() {
    durakSpaceEnabled = GetPrivateProfileIntA("Controls", "SpaceToggle", 1, ".\\crazy_loto_tweaks.ini");
    durakKeyFunctionRva = gameProfile->durakKeyFunction;
    const auto replacement = reinterpret_cast<std::uintptr_t>(&DurakKeyDown);
    for (std::size_t i = 0; i < gameProfile->durakCallCount; ++i) {
        const auto call = gameBase + gameProfile->durakCalls[i];
        const auto relative = static_cast<std::int32_t>(replacement - call - 5);
        WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(call + 1),
            &relative, sizeof(relative), nullptr);
    }
}
