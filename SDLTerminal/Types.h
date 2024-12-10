#pragma once
#include "GlobalLibs.h"

struct Vector2 {
	int x = 0;
	int y = 0;
};

struct Text {

	std::string text = "";
	SDL_Color color = { 0xff, 0xff, 0xff, 0xff };

	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_FRect rect = { };
	
};