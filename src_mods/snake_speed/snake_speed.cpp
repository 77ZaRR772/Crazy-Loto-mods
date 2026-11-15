#include <windows.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "../game_profile.hpp"

namespace {
int initialDelay = 5;
int minimumDelay = 0;
int speedUpEvery = 10;
int initialFood = 3;
int poopChance = 2;
int maxPoops = 5;

void readConfig() {
    FILE* file = std::fopen("crazy_loto_tweaks.ini", "r");
    if (!file) {
        return;
    }
    char line[128];
    while (std::fgets(line, sizeof(line), file)) {
        int value;
        if (std::sscanf(line, " InitialDelay = %d", &value) == 1) {
            initialDelay = std::clamp(value, 1, 100);
        }
        else if (std::sscanf(line, " MinimumDelay = %d", &value) == 1) {
            minimumDelay = std::clamp(value, 0, 100);
        }
        else if (std::sscanf(line, " SpeedUpEvery = %d", &value) == 1) {
            speedUpEvery = std::clamp(value, 0, 1000);
        }
        else if (std::sscanf(line, " InitialFood = %d", &value) == 1) {
            initialFood = std::clamp(value, 1, 100);
        }
        else if (std::sscanf(line, " PoopChance = %d", &value) == 1) {
            poopChance = std::clamp(value, 0, 100);
        }
        else if (std::sscanf(line, " MaxPoops = %d", &value) == 1) {
            maxPoops = std::clamp(value, 1, 100);
        }
    }
    std::fclose(file);
}

void writeIfChanged(std::uintptr_t address, const void* data, std::size_t size) {
    std::array<std::uint8_t, 64> current{};
    if (ReadProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(address),
            current.data(), size, nullptr) && std::memcmp(current.data(), data, size) == 0) {
        return;
    }
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(address), data, size, nullptr);
}

void writeByte(std::uintptr_t address, std::uint8_t value) {
    writeIfChanged(address, &value, 1);
}

void writeDword(std::uintptr_t address, std::uint32_t value) {
    writeIfChanged(address, &value, 4);
}

void applyPoopPatch() {
    std::array<std::uint8_t, 15> patch{};
    patch.fill(0x90);
    if (poopChance == 0) {
        patch[0] = 0xE9;
        const auto jump = static_cast<std::int32_t>(gameProfile->snakePoopEnd - (gameProfile->snakePoop + 5));
        std::memcpy(patch.data() + 1, &jump, sizeof(jump));
    }
    else if (poopChance < 100) {
        const std::uint8_t original[] = {
            0xB8, 0x32, 0, 0, 0, 0xE8, 0, 0, 0, 0,
            0x48, 0x75, 0x5B, 0x83, 0x7F
        };
        std::copy(std::begin(original), std::end(original), patch.begin());
        const std::uint32_t range = std::max(2, 100 / poopChance);
        std::memcpy(patch.data() + 1, &range, sizeof(range));
        const auto relative = static_cast<std::int32_t>(
            gameProfile->snakeRandomFunction - (gameProfile->snakePoop + 10));
        std::memcpy(patch.data() + 6, &relative, sizeof(relative));
    }
    writeIfChanged(gameBase + gameProfile->snakePoop, patch.data(), patch.size());
}

void applyPatch() {
    writeByte(gameBase + gameProfile->snakeInitialDelay, static_cast<std::uint8_t>(initialDelay));
    writeByte(gameBase + gameProfile->snakeMinimumDelay, static_cast<std::uint8_t>(minimumDelay));
    writeByte(gameBase + gameProfile->snakeSpeedUpJump, speedUpEvery == 0 ? 0xEB : 0x7E);
    writeDword(gameBase + gameProfile->snakeSpeedUpEvery, static_cast<std::uint32_t>(std::max(speedUpEvery, 1)));
    writeByte(gameBase + gameProfile->snakeInitialFood, static_cast<std::uint8_t>(initialFood));
    writeByte(gameBase + gameProfile->snakeMaxPoops, static_cast<std::uint8_t>(maxPoops));
    applyPoopPatch();
}
}

DWORD WINAPI snakeConfigThread(LPVOID) {
    while (true) {
        Sleep(500);
        readConfig();
        applyPatch();
    }
}

extern "C" void InitializeSnakeSpeed() {
    readConfig();
    applyPatch();
    CreateThread(nullptr, 0, snakeConfigThread, nullptr, 0, nullptr);
}
