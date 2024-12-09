#pragma once
#include "Window.h"

class Terminal {

public:

	Terminal();

	void Init();

private:

	Window window;
	SDL_Renderer* renderer = nullptr;

	SDL_Color bgcolor = { 0x0, 0x0, 0x0, 0xff };

	struct TextData {

		int fontsize = 0;

		TTF_Font* font = nullptr;

	} textdata;


	std::string userinput = "";
	std::vector<std::string> history = { };

};