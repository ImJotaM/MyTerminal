#pragma once
#include "GlobalLibs.h"

#define BLACK { 0x00, 0x00, 0x00, 0xff }
#define WHITE { 0xff, 0xff, 0xff, 0xff }
#define RED { 0xff, 0x00, 0x00, 0xff }
#define GREEN { 0x00, 0xff, 0x00, 0xff }
#define BLUE { 0x00, 0x00, 0xff, 0xff }

struct Vector2 {
	int x = 0;
	int y = 0;
};

struct Vector2f {
	int x = 0;
	int y = 0;
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

struct Text {

	std::string text = "";
	SDL_Color color = WHITE;
	SDL_Color shadecolor = BLACK;

};
