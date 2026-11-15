#include <windows.h>

#include "../game_profile.hpp"

extern "C" void InitializeFishTime() {
    const int configured = GetPrivateProfileIntA("Fish", "DurationMinutes", 3, ".\\crazy_loto_tweaks.ini");
    const int minutes = configured < 1 ? 1 : configured > 60 ? 60 : configured;
    const auto seconds = static_cast<std::uint32_t>(minutes * 60);
    WriteProcessMemory(GetCurrentProcess(),
        reinterpret_cast<void*>(gameBase + gameProfile->fishSeconds), &seconds, sizeof(seconds), nullptr);
}
