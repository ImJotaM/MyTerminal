#include "Terminal.h"

Terminal::Terminal() {
	
	if (SDL_Init(SDL_INIT_VIDEO)) {
		dependencies.SDL_VIDEO = DEP_OK;
	} else {
		dependencies.HandleErrors();
	}

	if (TTF_Init()) {
		dependencies.SDL_TTF = DEP_OK;
	}else{
		dependencies.HandleErrors();
	}

	GetScreenData();

	windata.title = "Terminal";
	windata.width = screen.width / 2;
	windata.height = screen.height / 2;
	windata.position.x = (screen.width - windata.width) / 2;
	windata.position.y = (screen.height - windata.height) / 2;
	windata.flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY;

	window = SDL_CreateWindow(windata.title, windata.width, windata.height, windata.flags);
	if (!window) {
		dependencies.HandleErrors();
	}

	renderer = SDL_CreateRenderer(window, NULL);
	if (!renderer) {
		dependencies.HandleErrors();
	}

	font.fontsize = 14;
	font.ttf_font = TTF_OpenFont("resources/CONSOLA.ttf", font.fontsize);
	if (!font.ttf_font) {
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
	}
	
	currentdir = fs::current_path();
	bgcolor = { 0xc, 0xc, 0xc, 0xff };

	SDL_Surface* tmp_font_surface = TTF_RenderGlyph_Shaded(font.ttf_font, ' ', WHITE, BLACK);
	font.width = tmp_font_surface->w;
	font.height = tmp_font_surface->h;
	SDL_DestroySurface(tmp_font_surface);

	columns = windata.width / font.width;
	rows = windata.height / font.height;

	cellmtrx.reserve(columns * rows);
	cellmtrx.resize(columns * rows, { });
	
	textcursor.width = font.width;
	textcursor.height = font.height;

	RegisterCommands();

}

Terminal::~Terminal() {

	userinput.clear();
	history.clear();
	cellmtrx.clear();

	TTF_CloseFont(font.ttf_font);

	for (auto& [character, texture] : charcache) {
		SDL_DestroyTexture(texture);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	dependencies.QUIT_ALL();

}

void Terminal::Init() {

	SDL_Event e;

	SDL_StartTextInput(window);

	while (!windata.shouldclose) {

		while (SDL_PollEvent(&e)) {

			if ((e.type == SDL_EVENT_QUIT) || (e.key.key == SDLK_ESCAPE && e.type == SDL_EVENT_KEY_DOWN)) {
				CloseWindow();
			}

			if (e.type == SDL_EVENT_TEXT_INPUT) {
				userinput += e.text.text;
			}

			if (e.key.key == SDLK_BACKSPACE && e.type == SDL_EVENT_KEY_DOWN) {
				if (!userinput.empty()) {
					userinput.pop_back();
				}
			}

			if (e.key.key == SDLK_RETURN && e.type == SDL_EVENT_KEY_DOWN) {
				HandleInput();
				history += currentdir.string() + "> " + userinput + "\n";
				userinput.clear();
			}

			if (e.window.type == SDL_EVENT_WINDOW_RESIZED) {
				windata.width = e.window.data1;
				windata.height = e.window.data2;

				columns = windata.width / font.width;
				rows = windata.height / font.height;
			}

		}

		ClearWindow();

		ClearMatrix();
		UpdateCellMatrix();
		DrawCellMatrix();

		SDL_RenderPresent(renderer);

	}

	SDL_StopTextInput(window);

}

void Terminal::GetScreenData() {

	SDL_Rect displaybounds = { };
	if (!SDL_GetDisplayBounds(1, &displaybounds)) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
	}

	screen.width = displaybounds.w;
	screen.height = displaybounds.h;

}

void Terminal::CloseWindow() {
	windata.shouldclose = true;
}

void Terminal::ClearWindow() {
	SDL_SetRenderDrawColor(renderer, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
	SDL_RenderClear(renderer);
}

void Terminal::UpdateCellMatrix() {

	Vector2 cursor = { 0, 0 };
	std::string out = "";

	out = history + currentdir.string() + "> " + userinput;

	for (char c : out) {

		size_t index = cursor.y * columns + cursor.x;

		if (c == '\n') {
			cursor.x = 0;
			cursor.y += 1;
			continue;
		}

		cellmtrx[index].character = c;
		cursor.x += 1;
		if (cursor.x == columns) {
			cursor.x = 0;
			cursor.y += 1;
		}

	}

	textcursor.x = cursor.x * font.width;
	textcursor.y = cursor.y * font.height;

}

void Terminal::DrawCellMatrix() {

	Vector2 cursor = { 0, 0 };

	for (Cell& cell : cellmtrx) {

		if (cell.character == '\n')
			continue;

		if (charcache.find(cell.character) == charcache.end()) {

			SDL_Surface* temp_surface = TTF_RenderGlyph_Shaded(font.ttf_font, cell.character, cell.color, BLACK);
			if (!temp_surface)
				continue;

			SDL_Texture* temp_texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			SDL_DestroySurface(temp_surface);
			if (!temp_texture)
				continue;

			charcache[cell.character] = temp_texture;
		}

		SDL_FRect rect = { cursor.x * font.width, cursor.y * font.height, font.width, font.height };
		SDL_RenderTexture(renderer, charcache[cell.character], nullptr, &rect);

		cursor.x += 1;
		if (cursor.x == columns) {
			cursor.x = 0;
			cursor.y += 1;
		}

	}

	SDL_FRect cursor_rect = { textcursor.x, textcursor.y, textcursor.width, textcursor.height };
	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(renderer, &cursor_rect);

}

void Terminal::ClearMatrix() {
	
	cellmtrx.clear();
	cellmtrx.resize(columns * rows, { });

}

void Terminal::HandleInput() {

	std::vector<std::string> tokens = {};

	std::stringstream ss(userinput);
	std::string token;

	while (ss >> token) {
		tokens.push_back(token);
	}

	auto mapIter = commandlist.find(tokens[0]);
	if (mapIter != commandlist.end()) {
		mapIter->second();
	}

}

void Terminal::RegisterCommands() {
	commandlist["cd"] = [this]() { COMMAND_CD(); };
}

