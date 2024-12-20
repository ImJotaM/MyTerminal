#pragma once
#include "GlobalLibs.h"

#define DEP_OK true

struct Dependencies {

	bool SDL_VIDEO = false;
	bool SDL_TTF = false;

	void HandleErrors() {

		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		throw SDL_GetError();
		QUIT_ALL();

	}

	void QUIT_ALL() {

		if (SDL_TTF)
			TTF_Quit();

		if (SDL_VIDEO)
			SDL_Quit();

	}

} inline dependencies;

#define BLACK { 0x00, 0x00, 0x00, 0xff }
#define WHITE { 0xff, 0xff, 0xff, 0xff }

struct Vector2 {
	int x = 0;
	int y = 0;
};

struct Vector2f {
	int x = 0;
	int y = 0;
};
