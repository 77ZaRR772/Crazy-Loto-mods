#pragma once

#include <cstddef>
#include <cstdint>

struct GameProfile {
    const char* name;
    std::uintptr_t noShakeX;
    std::uintptr_t noShakeY;
    std::uint32_t noShakeXPointer;
    std::uint32_t noShakeYPointer;
    std::uintptr_t snakeInitialDelay;
    std::uintptr_t snakeMinimumDelay;
    std::uintptr_t snakeSpeedUpJump;
    std::uintptr_t snakeSpeedUpEvery;
    std::uintptr_t snakeInitialFood;
    std::uintptr_t snakeMaxPoops;
    std::uintptr_t snakePoop;
    std::uintptr_t snakePoopEnd;
    std::uintptr_t snakeRandomFunction;
    std::uintptr_t durakKeyFunction;
    const std::uintptr_t* durakCalls;
    std::size_t durakCallCount;
    std::uintptr_t gunShakeCall;
    std::uintptr_t gunAmmo;
    std::uintptr_t gunOriginalShake;
    std::uintptr_t gunAimXPointer;
    std::uintptr_t gunAimYPointer;
    const std::uintptr_t* gunSlotReads;
    std::size_t gunSlotReadCount;
    std::uintptr_t fishSeconds;
    std::uintptr_t skipIntroCall;
    std::uintptr_t titleScene;
    std::uint32_t textStatePointer;
    std::uint32_t textOffset;
};

extern std::uintptr_t gameBase;
extern const GameProfile* gameProfile;

bool DetectGameVersion();
