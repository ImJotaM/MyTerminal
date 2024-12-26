#pragma once
#include "Types.h"

class Terminal {

public:

	Terminal();
	~Terminal();

	void Init();
	void Print(const std::string& msg, SDL_Color text_color = WHITE);

private:

	SDL_Renderer* renderer = nullptr;

	struct WindowData {

		SDL_Window* sdl_window = nullptr;

		const char* title = "Terminal";
		int width = 800;
		int height = 600;
		Vector2 position = { 0, 0 };
		const SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

		bool shouldclose = false;

	} window;

	struct Display {

		int width = 0;
		int height = 0;

		float scale = 0;

	} display;

	struct FontData {

		float fontsize = 0;

		int width = 0;
		int height = 0;

		TTF_Font* ttf_font = nullptr;

	} font;

	SDL_FRect textCursor = { 0.f, 0.f, 0.f, 0.f };

	std::vector<Text> history = { };
	std::string userinput = "";
	fs::path currentdir = "";
	SDL_Color bgcolor = { 0xc, 0xc, 0xc, 0xff };
	
	std::unordered_map<std::string, std::function<void(int, std::vector<std::string>)>> commandlist;

	void CloseWindow();
	void ClearWindow();

	void UpdateContent(std::vector<Text>& out);
	void DrawContent();

	SDL_Texture* texture = nullptr;

	void HandleInput();

	void RegisterCommands();

	void COMMAND_CD(int argc, std::vector<std::string>);
	void COMMAND_LS(int argc, std::vector<std::string>);
	void COMMAND_CLS(int argc, std::vector<std::string>);
	void COMMAND_MKDIR(int argc, std::vector<std::string>);
	void COMMAND_RM(int argc, std::vector<std::string>);

};
