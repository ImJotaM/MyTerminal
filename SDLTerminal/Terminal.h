#pragma once
#include "Window.h"

class Terminal {

public:

	Terminal();

	void Init();
	template<typename... Args>
	void Print(const std::string& text, Args... args);

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

template<typename ...Args>
inline void Terminal::Print(const std::string& text, Args... args) {

	std::ostringstream stream;

	stream << text;
	((stream << args), ...);

	history.emplace_back(stream.str());

}
