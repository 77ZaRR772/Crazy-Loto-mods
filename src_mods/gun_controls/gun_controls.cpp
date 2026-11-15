#include <windows.h>

#include "../game_profile.hpp"

int gunNoShake = 1;
std::uintptr_t gunGameBase;
std::uintptr_t gunOriginalShakeRva;
std::uintptr_t gunAimXPointerRva;
std::uintptr_t gunAimYPointerRva;

extern "C" __declspec(naked) void GunShake() {
    __asm__ volatile(
        "cmpl $0, _gunNoShake\n"
        "je 1f\n"
        "mov _gunGameBase, %eax\n"
        "add _gunAimXPointerRva, %eax\n"
        "mov (%eax), %eax\n"
        "movl $0, (%eax)\n"
        "mov _gunGameBase, %eax\n"
        "add _gunAimYPointerRva, %eax\n"
        "mov (%eax), %eax\n"
        "movl $0, (%eax)\n"
        "ret\n"
        "1:\n"
        "mov _gunGameBase, %eax\n"
        "add _gunOriginalShakeRva, %eax\n"
        "jmp *%eax\n"
    );
}

extern "C" void InitializeGunControls() {
    gunNoShake = GetPrivateProfileIntA("Gun", "NoShake", 1, ".\\crazy_loto_tweaks.ini");
    const int configuredAmmo = GetPrivateProfileIntA("Gun", "Ammo", 5, ".\\crazy_loto_tweaks.ini");
    const auto ammo = static_cast<std::uint16_t>(configuredAmmo < 1 ? 1 : configuredAmmo > 999 ? 999 : configuredAmmo);
    gunGameBase = gameBase;
    gunOriginalShakeRva = gameProfile->gunOriginalShake;
    gunAimXPointerRva = gameProfile->gunAimXPointer;
    gunAimYPointerRva = gameProfile->gunAimYPointer;
    const auto shakeCall = gameBase + gameProfile->gunShakeCall;
    const auto replacement = reinterpret_cast<std::uintptr_t>(&GunShake);
    const auto relative = static_cast<std::int32_t>(replacement - shakeCall - 5);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(shakeCall + 1), &relative, 4, nullptr);
    WriteProcessMemory(GetCurrentProcess(), reinterpret_cast<void*>(gameBase + gameProfile->gunAmmo),
        &ammo, 2, nullptr);
    if (ammo > 5) {
        const std::uint8_t zeroSlot[] = {0x31, 0xD2, 0x90};
        for (std::size_t i = 0; i < gameProfile->gunSlotReadCount; ++i) {
            WriteProcessMemory(GetCurrentProcess(),
                reinterpret_cast<void*>(gameBase + gameProfile->gunSlotReads[i]), zeroSlot, 3, nullptr);
        }
    }
}
