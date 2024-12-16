#include "Terminal.h"

Terminal::Terminal() {
	
	GetScreenData();

	windata.win_title = "Terminal";
	windata.win_width = screen.width / 2;
	windata.win_height = screen.height / 2;
	windata.win_position.x = (screen.width - windata.win_width) / 2;
	windata.win_position.y = (screen.height - windata.win_height) / 2;
	windata.win_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

	window.Configure(windata.win_title, windata.win_width, windata.win_height, windata.win_position, windata.win_flags);
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

	columns = windata.win_width / textdata.fontsize;
	rows = windata.win_height / textdata.fontsize;

	cellmtrx.resize(rows);

	for (std::vector<Cell>& line : cellmtrx) {
		line.resize(columns);
	}

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

		DrawCellMatrix();

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

void Terminal::DrawCellMatrix() {

	

	if (cell_surface)
		SDL_DestroySurface(cell_surface);

	if (cell_texture)
		SDL_DestroyTexture(cell_texture);

	for (size_t i = 0; i < cellmtrx.size(); i++){
		std::vector<Cell>& line = cellmtrx[i];
		for (size_t j = 0; j < line.size(); j++){
			Cell& cell = line[j];
			
			cell_surface = TTF_RenderGlyph_Shaded(textdata.font, cell.character, cell.color, BLACK);

			if (cell_surface) {

				cell_texture = SDL_CreateTextureFromSurface(renderer, cell_surface);
				SDL_FRect rect = { j * textdata.fontsize, i * textdata.fontsize, cell_surface->w, cell_surface->h };
				SDL_RenderTexture(renderer, cell_texture, nullptr, &rect);

			}

		}
	}

}

