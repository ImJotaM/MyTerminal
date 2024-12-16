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

	struct WindowData {

		const char* win_title = "";
		int win_width = 0;
		int win_height = 0;

		Vector2 win_position = { };
		SDL_WindowFlags win_flags = 0x0;

	} windata;

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

	struct Cell {
		char character = ' ';
		SDL_Color color = WHITE;
	};

	int columns = 80;
	int rows = 60;

	std::vector<std::vector<Cell>> cellmtrx = { };

	SDL_Surface* cell_surface = nullptr;
	SDL_Texture* cell_texture = nullptr;

	void GetScreenData();

	void DrawCellMatrix();

};
