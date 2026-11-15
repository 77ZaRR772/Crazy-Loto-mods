#include <windows.h>

#include "../game_profile.hpp"

namespace {
void applyConfig() {
    const int configured = GetPrivateProfileIntA("Text", "Enabled", -1, ".\\crazy_loto_tweaks.ini");
    if (configured != 0 && configured != 1) {
        return;
    }
    void* state = nullptr;
    if (!ReadProcessMemory(GetCurrentProcess(),
            reinterpret_cast<void*>(gameBase + gameProfile->textStatePointer - 0x400000),
            &state, sizeof(state), nullptr) || !state) {
        return;
    }
    const auto value = static_cast<std::uint8_t>(configured);
    auto* target = static_cast<std::uint8_t*>(state) + gameProfile->textOffset;
    std::uint8_t current = 0;
    if (ReadProcessMemory(GetCurrentProcess(), target, &current, sizeof(current), nullptr) &&
        current == value) {
        return;
    }
    WriteProcessMemory(GetCurrentProcess(), target, &value, sizeof(value), nullptr);
}
}

DWORD WINAPI textConfigThread(LPVOID) {
    while (true) {
        applyConfig();
        Sleep(500);
    }
}

extern "C" void InitializeTextSettings() {
    CreateThread(nullptr, 0, textConfigThread, nullptr, 0, nullptr);
}
