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

	struct Text {

		std::string text = "";
		SDL_Color color = WHITE;
		SDL_Color shadecolor = BLACK;

	};

	struct TextCache {
		Text data = { };
		SDL_Texture* texture = nullptr;
		Vector2f size = { 0.f, 0.f };
	};

	SDL_FRect textCursor = { 0.f, 0.f, 0.f, 0.f };

	std::vector<Text> history = { };
	std::string userinput = "";
	fs::path currentdir = "";
	SDL_Color bgcolor = { 0xc, 0xc, 0xc, 0xff };
	
	std::unordered_map<std::string, std::function<void(int, std::vector<std::string>)>> commandlist;

	void HandleEvents(SDL_Event& event);
	void KeyHandler(SDL_KeyboardEvent& key);

	void CloseWindow();
	void ClearWindow();

	std::vector<TextCache> textCache = { };
	void UpdateTextCache(const std::vector<Text>& out);

	void FormatContent(std::vector<Text>& out);
	void DrawContent();

	void HandleInput();

	void RegisterCommands();

	void COMMAND_CD(int argc, std::vector<std::string>);
	void COMMAND_LS(int argc, std::vector<std::string>);
	void COMMAND_CLS(int argc, std::vector<std::string>);
	void COMMAND_MKDIR(int argc, std::vector<std::string>);
	void COMMAND_RM(int argc, std::vector<std::string>);

};
