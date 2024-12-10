#include "Terminal.h"

Terminal::Terminal() {
	
	GetScreenData();

	const char* win_title = "";
	int win_width = 0;
	int win_height = 0;
	Vector2 win_position = { };
	SDL_WindowFlags win_flags = 0x0;

	win_title = "Terminal";
	win_width = screen.width / 2;
	win_height = screen.height / 2;
	win_position.x = (screen.width - win_width) / 2;
	win_position.y = (screen.height - win_height) / 2;
	win_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

	window.Configure(win_title, win_width, win_height, win_position, win_flags);
	window.Create();

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		std::exit(EXIT_FAILURE);
	}

	textdata.fontsize = 14;
	textdata.wrapwidth = window.GetWidth();
	textdata.font = TTF_OpenFont("resources/Segoe UI.ttf", textdata.fontsize);
	if (!textdata.font) {
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
	}
	
	currentdir = fs::current_path();
	bgcolor = { 0xc, 0xc, 0xc, 0xff };

}

Terminal::~Terminal() {

	SDL_DestroyTexture(userinput.texture);
	SDL_DestroyTexture(history.texture);
	TTF_CloseFont(textdata.font);

	SDL_DestroyRenderer(renderer);
	window.Destroy();

}

void Terminal::Init() {

	SDL_Event e;

	SDL_StartTextInput(window);

	while (!window.shouldClose()) {

		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_EVENT_QUIT) {
				window.Close();
			}

			if (e.type == SDL_EVENT_TEXT_INPUT) {
				userinput.text += e.text.text;
			}

			if (e.key.key == SDLK_ESCAPE && e.type == SDL_EVENT_KEY_DOWN) {
				window.Close();
			}

		}

		window.Clear(renderer, bgcolor);

		DrawHistory();
		DrawUserInput();

		SDL_RenderPresent(renderer);

	}

}

void Terminal::GetScreenData() {

	SDL_Rect displaybounds = { };
	if (!SDL_GetDisplayBounds(1, &displaybounds)) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
	}

	screen.width = displaybounds.w;
	screen.height = displaybounds.h;

}

void Terminal::DrawHistory() {

	if (history.texture) {
		SDL_DestroyTexture(userinput.texture);
	}
	if (history.surface) {
		SDL_DestroySurface(userinput.surface);
	}

	history.surface = TTF_RenderText_Blended_Wrapped(textdata.font, history.text.c_str(), history.text.size(), { 0xff, 0xff, 0xff, 0xff }, textdata.wrapwidth);
	if (history.surface) {
		history.texture = SDL_CreateTextureFromSurface(renderer, history.surface);
		history.rect = { 0, 0, static_cast<float>(history.surface->w), static_cast<float>(history.surface->h) };
		SDL_DestroySurface(history.surface);
	}

	SDL_RenderTexture(renderer, history.texture, nullptr, &history.rect);

}

void Terminal::DrawUserInput() {

	std::string out = "  " + currentdir.string() + "> " + userinput.text;

	if (userinput.texture) {
		SDL_DestroyTexture(userinput.texture);
	}
	if (userinput.surface) {
		SDL_DestroySurface(userinput.surface);
	}

	userinput.surface = TTF_RenderText_Blended_Wrapped(textdata.font, out.c_str(), out.size(), { 0xff, 0xff, 0xff, 0xff }, textdata.wrapwidth - 20);
	if (userinput.surface) {
		userinput.texture = SDL_CreateTextureFromSurface(renderer, userinput.surface);
		userinput.rect = { 0, 0, static_cast<float>(userinput.surface->w), static_cast<float>(userinput.surface->h) };
		SDL_DestroySurface(userinput.surface);
	}

	SDL_RenderTexture(renderer, userinput.texture, nullptr, &userinput.rect);

}
