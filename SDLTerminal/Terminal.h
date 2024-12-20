#pragma once
#include "Types.h"

class Terminal {

public:

	Terminal();
	~Terminal();

	void Init();

private:

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	struct WindowData {

		const char* title = "";
		int width = 0;
		int height = 0;

		Vector2 position = { };
		SDL_WindowFlags flags = 0x0;

		bool shouldclose = false;

	} windata;

	struct Screen {

		int width;
		int height;

	} screen;

	struct FontData {

		int fontsize = 0;

		int width = 0;
		int height = 0;

		TTF_Font* ttf_font = nullptr;

	} font;

	struct TextCursor {

		int x = 0;
		int y = 0;

		int width = 0;
		int height = 0;

	} textcursor;

	struct Cell {
		char character = ' ';
		SDL_Color color = WHITE;
	};

	std::vector<Cell> cellmtrx = { };

	std::string history = "";
	std::string userinput = "";
	fs::path currentdir = "";
	SDL_Color bgcolor = { 0x0, 0x0, 0x0, 0xff };
	
	int columns = 0;
	int rows = 0;

	std::unordered_map<char, SDL_Texture*> charcache;

	std::unordered_map<std::string, std::function<void()>> commandlist;

	void GetScreenData();
	void CloseWindow();
	void ClearWindow();

	void UpdateCellMatrix();
	void DrawCellMatrix();
	void ClearMatrix();

	void HandleInput();

	void RegisterCommands();

	void COMMAND_CD();

};
