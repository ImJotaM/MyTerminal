#pragma once
#include "Types.h"

class Terminal {

public:

	Terminal();
	~Terminal();

	void Init();
	void Print(const std::string& msg, SDL_Color text_color = WHITE);

private:

	enum TerminalMode {

		NONE = -1,
		TERMINAL = 0,
		EDITOR = 1

	} terminal_mode = NONE;

	struct WindowData {

		SDL_Window* sdl_window = nullptr;

		const char* title = "Terminal";
		int width = 800;
		int height = 600;
		Vector2 position = { 0, 0 };
		const SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;
		SDL_Color bgcolor = { 0xc, 0xc, 0xc, 0xff };

		bool shouldclose = false;

	} window;

	struct Display {

		int width = 0;
		int height = 0;

		float scale = 0;

	} display;

	struct Font {

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

	struct ScrollData {

		int scrollOffset = 0;
		int scrollStep = 0;

	} scroll;

	struct TextCursor {

		SDL_FRect frect = { 0.f, 0.f, 0.f, 0.f };
		Uint64 blinkInterval = 600;
		bool cursorVisible = true;
		Uint64 lastBlinkTime = 0;

	} textCursor;

	SDL_Renderer* renderer = nullptr;

	std::vector<Text> content = { };
	std::string userinput = "";
	fs::path currentdir = "";
	std::vector<Text> out = { };
	std::vector<TextCache> textCache = { };

	Uint64 currentTime = 0;
	void UpdateCurrentTime();

	std::unordered_map<std::string, std::function<void(int, std::vector<std::string>)>> commandlist;

	void HandleEvents(SDL_Event& event);
	void KeyHandler(SDL_KeyboardEvent& key);
	void TextInputHandler(SDL_TextInputEvent& text);

	void CloseWindow();
	void ClearWindow();
	void ResizeWindow(int win_width, int win_height);
	
	void UpdateContent();
	void TerminalFormatContent();
	void UpdateTextCache();
	
	void DrawContent();

	void HandleInput();

	void UpdateView();
	bool IsInputVisible = true;

	void RegisterCommands();

	void COMMAND_CD      (int argc, std::vector<std::string> argv);
	void COMMAND_LS      (int argc, std::vector<std::string> argv);
	void COMMAND_CLS     (int argc, std::vector<std::string> argv);
	void COMMAND_MKDIR   (int argc, std::vector<std::string> argv);
	void COMMAND_RM      (int argc, std::vector<std::string> argv);
	void COMMAND_MKFILE  (int argc, std::vector<std::string> argv);
	void COMMAND_RUN     (int argc, std::vector<std::string> argv);
	void COMMAND_EDIT    (int argc, std::vector<std::string> argv);

	// ** Editor mode **

	fs::path currentFileDir = "";
	std::fstream currentFile;
	std::vector<std::string> fileBuffer = { };
	std::vector<Text> fileContent = { };

	void StartEditorMode(fs::path& filedir);
	void ExitEditorMode();
	
	void EditorFormatContent();

};
