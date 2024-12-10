#pragma once
#include "Window.h"

class Terminal {

public:

	Terminal();
	~Terminal();

	void Init();
	template<typename... Args>
	void Print(const std::string& text, Args... args);

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

	Text userinput = { };
	Text history = { };
	fs::path currentdir = "";

	void GetScreenData();

	void DrawHistory();
	void DrawUserInput();

};

template<typename ...Args>
inline void Terminal::Print(const std::string& text, Args... args) {

	std::ostringstream stream;

	stream << text;
	((stream << args), ...);

	history.text += stream.str();
}
