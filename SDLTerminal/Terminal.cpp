#include "Terminal.h"

Terminal::Terminal() {

	if (!TTF_Init()) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	renderer = window.GetRenderer();

	bgcolor = { 0xc, 0xc, 0xc, 0xff };

	textdata.fontsize = 16;
	textdata.font = TTF_OpenFont("resources/Segoe UI.ttf", textdata.fontsize);

	if (!textdata.font) {
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
	}

}

void Terminal::Init() {

	SDL_Event e;

	SDL_Surface* text_surface = nullptr;
	SDL_Texture* texture = nullptr;
	SDL_FRect textrect = { };

	userinput = "ImJm\n nanana";

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

		text_surface = TTF_RenderText_Blended_Wrapped(textdata.font, userinput.c_str(), userinput.size(), { 0xff, 0xff, 0xff, 0xff }, 200);
		if (text_surface) {
			texture = SDL_CreateTextureFromSurface(renderer, text_surface);
			textrect = { 0, 0, static_cast<float>(text_surface->w), static_cast<float>(text_surface->h) };
			SDL_DestroySurface(text_surface);
		}

		window.Clear();

		SDL_RenderTexture(renderer, texture, nullptr, &textrect);

		window.RenderPresent();

	}

	TTF_Quit();

}
