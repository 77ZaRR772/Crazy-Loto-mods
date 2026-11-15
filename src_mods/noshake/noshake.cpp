#include <windows.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "../game_profile.hpp"

namespace {
float shakeScale = 1.0f;
float shakeScaleX = 1.0f;
float shakeScaleY = 1.0f;

struct Patch {
    std::uintptr_t address;
    float scale;
    std::array<std::uint8_t, 29> original;
    std::array<std::uint8_t, 29> disabled;
};

Patch makePatch(std::uintptr_t address, std::uint32_t offsetPointer,
                const std::array<std::uint8_t, 29>& original) {
    Patch patch{address, 1.0f, original, {}};
    patch.disabled.fill(0x90);
    const std::uint8_t prefix[] = {0x8B, 0x15};
    const std::uint8_t storeZero[] = {0xC7, 0x02, 0, 0, 0, 0};
    std::copy(std::begin(prefix), std::end(prefix), patch.disabled.begin());
    std::memcpy(patch.disabled.data() + 2, &offsetPointer, sizeof(offsetPointer));
    std::copy(std::begin(storeZero), std::end(storeZero), patch.disabled.begin() + 6);
    return patch;
}

void readConfig() {
    FILE* file = std::fopen("crazy_loto_tweaks.ini", "r");
    if (!file) {
        return;
    }
    char line[128];
    while (std::fgets(line, sizeof(line), file)) {
        float value;
        if (std::sscanf(line, " ShakeScale = %f", &value) == 1) {
            shakeScale = std::clamp(value, 0.0f, 1.0f);
        }
        else if (std::sscanf(line, " ShakeScaleX = %f", &value) == 1) {
            shakeScaleX = std::clamp(value, 0.0f, 1.0f);
        }
        else if (std::sscanf(line, " ShakeScaleY = %f", &value) == 1) {
            shakeScaleY = std::clamp(value, 0.0f, 1.0f);
        }
    }
    std::fclose(file);
}

void applyPatch() {
    constexpr std::array<std::uint8_t, 29> demoX = {
        0xB8, 0x1F, 0, 0, 0, 0xE8, 0xCE, 0x63, 0xFB, 0xFF,
        0x8B, 0x15, 0xE0, 0xEC, 0x46, 0, 0x03, 0x02, 0x83, 0xE8, 0x0F,
        0x8B, 0x15, 0xE0, 0xEC, 0x46, 0, 0x89, 0x02
    };
    constexpr std::array<std::uint8_t, 29> demoY = {
        0xB8, 0x1F, 0, 0, 0, 0xE8, 0xB1, 0x63, 0xFB, 0xFF,
        0x8B, 0x15, 0xA4, 0xEC, 0x46, 0, 0x03, 0x02, 0x83, 0xE8, 0x0F,
        0x8B, 0x15, 0xA4, 0xEC, 0x46, 0, 0x89, 0x02
    };
    constexpr std::array<std::uint8_t, 29> fullX = {
        0xB8, 0x1F, 0, 0, 0, 0xE8, 0xBE, 0x15, 0xFB, 0xFF,
        0x8B, 0x15, 0x54, 0x0D, 0x4D, 0, 0x03, 0x02, 0x83, 0xE8, 0x0F,
        0x8B, 0x15, 0x54, 0x0D, 0x4D, 0, 0x89, 0x02
    };
    constexpr std::array<std::uint8_t, 29> fullY = {
        0xB8, 0x1F, 0, 0, 0, 0xE8, 0xA1, 0x15, 0xFB, 0xFF,
        0x8B, 0x15, 0x14, 0x0D, 0x4D, 0, 0x03, 0x02, 0x83, 0xE8, 0x0F,
        0x8B, 0x15, 0x14, 0x0D, 0x4D, 0, 0x89, 0x02
    };
    const bool full = gameProfile->skipIntroCall == 0xC75B1;
    Patch patches[] = {
        makePatch(gameBase + gameProfile->noShakeX, gameProfile->noShakeXPointer, full ? fullX : demoX),
        makePatch(gameBase + gameProfile->noShakeY, gameProfile->noShakeYPointer, full ? fullY : demoY)
    };
    patches[0].scale = shakeScale * shakeScaleX;
    patches[1].scale = shakeScale * shakeScaleY;
    for (auto& patch : patches) {
        auto adjusted = patch.original;
        const auto amplitude = static_cast<std::uint8_t>(std::lround(15.0f * patch.scale));
        const std::uint32_t randomRange = amplitude * 2 + 1;
        std::memcpy(adjusted.data() + 1, &randomRange, sizeof(randomRange));
        adjusted[20] = amplitude;
        const auto& bytes = amplitude == 0 ? patch.disabled : adjusted;
        std::array<std::uint8_t, 29> current{};
        if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(patch.address),
                current.data(), current.size(), nullptr) &&
            std::memcmp(current.data(), bytes.data(), bytes.size()) == 0) {
            continue;
        }
        WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(patch.address),
            bytes.data(), bytes.size(), nullptr);
    }
}
}

DWORD WINAPI noShakeConfigThread(LPVOID) {
    while (true) {
        Sleep(500);
        readConfig();
        applyPatch();
    }
}

extern "C" void InitializeNoShake() {
    readConfig();
    applyPatch();
    CreateThread(nullptr, 0, noShakeConfigThread, nullptr, 0, nullptr);
}
