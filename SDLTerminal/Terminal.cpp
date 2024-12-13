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
	textdata.font = TTF_OpenFont("resources/CONSOLA.ttf", textdata.fontsize);
	if (!textdata.font) {
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
	}
	
	currentdir = fs::current_path();
	bgcolor = { 0xc, 0xc, 0xc, 0xff };

}

Terminal::~Terminal() {

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
				userinput += e.text.text;
			}

			if (e.key.key == SDLK_ESCAPE && e.type == SDL_EVENT_KEY_DOWN) {
				window.Close();
			}

		}
		
		std::string out = "  " + currentdir.string() + "> " + userinput;
		int mw = 0;
		size_t ml = 0;

		TTF_MeasureString(textdata.font, out.c_str(), out.size(), textdata.wrapwidth, &mw, &ml);

		if (!out.empty() && out.size() >= ml && mw >= textdata.wrapwidth) {
			out[ml] = '\n';
		}

		window.Clear(renderer, bgcolor);

		SDL_Surface* surface = TTF_RenderText_Blended_Wrapped(textdata.font, out.c_str(), out.size(), WHITE, textdata.wrapwidth + 14);
		
		if (surface) {

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FRect rect = { 0, 0, surface->w, surface->h };
			SDL_RenderTexture(renderer, texture, nullptr, &rect);
			
			SDL_DestroySurface(surface);
			SDL_DestroyTexture(texture);

		}

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

