#include "Terminal.h"

Terminal::Terminal() {

	renderer = window.GetRenderer();

	bgcolor = { 0xc, 0xc, 0xc, 0xff };

	textdata.fontsize = 16;

}

void Terminal::Init() {

	if (!TTF_Init()) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	TTF_Font* font;
	font = TTF_OpenFont("resources/Segoe UI.ttf", 16);

	if (!font) {
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
	}

	SDL_Event e;

	std::string text = "Tampa";
	SDL_Surface* text_surface = nullptr;

	text_surface = TTF_RenderText_Blended(font, text.c_str(), text.size(), { 0xff, 0xff, 0xff, 0xff });
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, text_surface);

	SDL_FRect textrect = { 100, 100, text_surface->w, text_surface->h };
	SDL_DestroySurface(text_surface);

	SDL_StartTextInput(window);

	while (!window.shouldClose()) {

		while (SDL_PollEvent(&e)) {

			if (e.type == SDL_EVENT_QUIT) {
				window.Close();
			}

			if (e.type == SDL_EVENT_TEXT_INPUT) {
				userinput += e.text.text;
			}

			if (e.key.key == SDLK_ESCAPE && e.type == SDL_EVENT_KEY_DOWN) {
				window.Close();
			}

		}

		window.Clear();

		SDL_RenderTexture(renderer, texture, nullptr, &textrect);

		window.RenderPresent();

	}

	TTF_Quit();

}
