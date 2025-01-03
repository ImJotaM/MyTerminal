#pragma once
#include "GlobalLibs.h"

inline const SDL_Color BLACK        = { 0x00, 0x00, 0x00, 0xff };
inline const SDL_Color WHITE        = { 0xff, 0xff, 0xff, 0xff };
inline const SDL_Color GREY         = { 0x80, 0x80, 0x80, 0xff };

inline const SDL_Color RED          = { 0xff, 0x00, 0x00, 0xff };
inline const SDL_Color GREEN        = { 0x00, 0xff, 0x00, 0xff };
inline const SDL_Color BLUE         = { 0x00, 0x00, 0xff, 0xff };

inline const SDL_Color DARK_RED     = { 0x80, 0x00, 0x00, 0xff };
inline const SDL_Color DARK_GREEN   = { 0x00, 0x80, 0x00, 0xff };
inline const SDL_Color DARK_BLUE    = { 0x00, 0x00, 0x80, 0xff };

inline const SDL_Color LIGHT_RED    = { 0xff, 0x40, 0x40, 0xff };
inline const SDL_Color LIGHT_GREEN  = { 0x40, 0xff, 0x40, 0xff };
inline const SDL_Color LIGHT_BLUE   = { 0x40, 0x40, 0xff, 0xff };

struct Vector2 {
	int x = 0;
	int y = 0;
};

struct Vector2f {
	float x = 0;
	float y = 0;
};

struct Rect {

	int x = 0;
	int y = 0;
	int width = 0;
	int height = 0;

	operator SDL_FRect() {
		return { static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height) };
	}

};
