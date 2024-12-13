#pragma once
#include "Window.h"

class Terminal {

public:

	Terminal();
	~Terminal();

	void Init();

private:

	Window window;
	SDL_Renderer* renderer = nullptr;

	struct Screen {
		int width;
		int height;
	} screen;

	struct TextData {

		int fontsize = 0;
		int wrapwidth = 0;

		TTF_Font* font = nullptr;

	} textdata;


	SDL_Color bgcolor = { 0x0, 0x0, 0x0, 0xff };

	std::string userinput = "";
	fs::path currentdir = "";

	void GetScreenData();

};
