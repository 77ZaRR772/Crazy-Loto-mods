#include "game_profile.hpp"

#include <windows.h>

#include <array>
#include <cstdio>

std::uintptr_t gameBase;
const GameProfile* gameProfile;

namespace {
constexpr std::uintptr_t demoDurakCalls[] = {
    0x5E339, 0x5E56A, 0x5E603, 0x5EED0,
    0x5F2C4, 0x5FAA9, 0x5FE0B, 0x6057D,
    0x609F9, 0x60F08, 0x61008, 0x610FE,
    0x611BD, 0x61283, 0x61330, 0x613A4
};
constexpr std::uintptr_t fullDurakCalls[] = {
    0x92E85, 0x930B6, 0x9314F, 0x93A1C,
    0x93E10, 0x945F5, 0x94957, 0x950C9,
    0x95545, 0x95A54, 0x95B54, 0x95C4A,
    0x95D09, 0x95DCF, 0x95E7C, 0x95EF0
};
constexpr std::uintptr_t demoGunSlotReads[] = {0x50D54, 0x50D6A, 0x50D8A, 0x50DC3};
constexpr std::uintptr_t fullGunSlotReads[] = {0x56378, 0x5638E, 0x563AE, 0x563E7};

const GameProfile demo = {
    "demo",
    0x4C58C, 0x4C5A9, 0x46ECE0, 0x46ECA4,
    0x510BC, 0x51638, 0x51639, 0x51644, 0x511D0, 0x51CBD, 0x51CAD, 0x51D15, 0x2964,
    0x44B4C, demoDurakCalls, std::size(demoDurakCalls),
    0x501E8, 0x500D5, 0x4B5A0, 0x6ECE0, 0x6ECA4,
    demoGunSlotReads, std::size(demoGunSlotReads),
    0x4CEFC, 0x671E1, 0x66154, 0x46EE4C, 0x2C
};
const GameProfile full = {
    "full",
    0x514C4, 0x514E1, 0x4D0D54, 0x4D0D14,
    0x56703, 0x56CAC, 0x56CAD, 0x56CB8, 0x56817, 0x57331, 0x57321, 0x57389, 0x2A8C,
    0x47AE0, fullDurakCalls, std::size(fullDurakCalls),
    0x5580C, 0x556D4, 0x4F1CC, 0xD0D54, 0xD0D14,
    fullGunSlotReads, std::size(fullGunSlotReads),
    0x51E57, 0xC75B1, 0xC5240, 0x4D0ED0, 0x2C
};

std::uint64_t executableHash() {
    char path[MAX_PATH];
    if (!GetModuleFileNameA(nullptr, path, std::size(path))) {
        return 0;
    }
    FILE* file = std::fopen(path, "rb");
    if (!file) {
        return 0;
    }
    std::uint64_t hash = 14695981039346656037ULL;
    std::array<unsigned char, 16384> buffer{};
    while (const auto count = std::fread(buffer.data(), 1, buffer.size(), file)) {
        for (std::size_t i = 0; i < count; ++i) {
            hash ^= buffer[i];
            hash *= 1099511628211ULL;
        }
    }
    std::fclose(file);
    return hash;
}
}

bool DetectGameVersion() {
    switch (executableHash()) {
    case 0x3BCCD9D9E739866AULL:
        gameProfile = &demo;
        break;
    case 0x613781F9DF6DF834ULL:
        gameProfile = &full;
        break;
    default:
        return false;
    }
    gameBase = reinterpret_cast<std::uintptr_t>(GetModuleHandleA(nullptr));
    return true;
}
