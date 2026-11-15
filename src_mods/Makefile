CXX = i686-w64-mingw32-g++
CXXFLAGS = -std=c++17 -O2 -shared -static -static-libgcc -static-libstdc++
SOURCES = crazy_loto_tweaks.cpp \
	game_profile.cpp \
	noshake/noshake.cpp \
	snake_speed/snake_speed.cpp \
	durak_controls/durak_controls.cpp \
	gun_controls/gun_controls.cpp \
	fish_time/fish_time.cpp \
	skip_intro/skip_intro.cpp \
	text_settings/text_settings.cpp

all: CrazyLotoTweaks.asi

CrazyLotoTweaks.asi: $(SOURCES) game_profile.hpp crazy_loto_tweaks.def
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) crazy_loto_tweaks.def

clean:
	rm -f CrazyLotoTweaks.asi
