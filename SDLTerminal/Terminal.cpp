#include "Terminal.h"

Terminal::Terminal() {

	if (!SDL_Init(SDL_INIT_VIDEO)) {

		SDL_Log("STATUS: SDL_Video -> [ERROR].");
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Log("");
		
		std::exit(EXIT_FAILURE);

	} else {
		SDL_Log("STATUS: SDL_Video -> [OK].");
	}

	if (!TTF_Init()) {
		
		SDL_Log("STATUS: SDL_TTF -> [ERROR].");
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Log("");

		SDL_Quit();
		std::exit(EXIT_FAILURE);

	} else {
		SDL_Log("STATUS: SDL_TTF -> [OK].");
	}

	SDL_DisplayID displayId = SDL_GetPrimaryDisplay();

	SDL_Log("");
	if (!displayId) {

		SDL_Log("STATUS: Display -> [ERROR].");
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Log("INFO: Using default window size -> [800 x 600].");
		SDL_Log("INFO: Using default window position -> [X: 0 , Y: 0].");
		SDL_Log("");

	} else {

		SDL_Rect displayBounds = { };
		if (!SDL_GetDisplayBounds(displayId, &displayBounds)) {

			SDL_Log("STATUS: Display -> [ERROR].");
			SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
			SDL_Log("INFO: Using default window size -> [800 x 600].");
			SDL_Log("INFO: Using default window position -> [X: 0 , Y: 0].");
			SDL_Log("");


		} else {

			display.width = displayBounds.w;
			display.height = displayBounds.h;
			display.scale = SDL_GetDisplayContentScale(displayId);

			window.width = static_cast<int>((display.width / 2) * display.scale);
			window.height = static_cast<int>((display.height / 2) * display.scale);

			window.position.x = static_cast<int>(((display.width - window.width) / 2) * display.scale);
			window.position.y = static_cast<int>(((display.height - window.height) / 2) * display.scale);

			SDL_Log("STATUS: Display -> [OK].");
			SDL_Log("INFO: Display size -> [%d x %d]", display.width, display.height);
			SDL_Log("INFO: Display scale -> [%.0f%%]", display.scale * 100);
			SDL_Log("");
			
		}

	}

	window.sdl_window = SDL_CreateWindow(window.title, window.width, window.height, window.flags);

	if (!window.sdl_window) {

		SDL_Log("STATUS: Window -> [ERROR]");
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Log("");

		TTF_Quit();
		SDL_Quit();
		std::exit(EXIT_FAILURE);

	} else {

		SDL_Log("STATUS: Window -> [OK]");
		SDL_Log("INFO: Using window size -> [%d x %d]", window.width, window.height);
		SDL_Log("INFO: Using window position -> [X: %d , Y: %d]", window.position.x, window.position.y);
		SDL_Log("");

	}

	renderer = SDL_CreateRenderer(window.sdl_window, NULL);
	if (!renderer) {

		SDL_Log("STATUS: Window -> [ERROR]");
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Log("");

		SDL_DestroyWindow(window.sdl_window);
		TTF_Quit();
		SDL_Quit();
		std::exit(EXIT_FAILURE);

	} else {
		SDL_Log("STATUS: Renderer -> [OK]");
	}

	font.fontsize = 14 * display.scale;
	font.ttf_font = TTF_OpenFont("resources/CONSOLA.ttf", font.fontsize);

	SDL_Log("");
	if (!font.ttf_font) {
		
		SDL_Log("STATUS: Default font (CONSOLA) -> [ERROR]");
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
		SDL_Log("");
		
	} else {

		SDL_Surface* tmp_font_surface = TTF_RenderGlyph_Shaded(font.ttf_font, ' ', BLACK, BLACK);
		font.width = tmp_font_surface->w;
		font.height = tmp_font_surface->h;
		SDL_DestroySurface(tmp_font_surface);

		SDL_Log("STATUS: Default font (CONSOLA) -> [OK]");
		SDL_Log("INFO: Using font size (CONSOLA) -> [%.2fpx]", font.fontsize);
		SDL_Log("INFO: Using font dimensions (CONSOLA) -> [%d x %d]", font.width, font.height);
		SDL_Log("");

	}

	currentdir = fs::current_path();

	textCursor.w = static_cast<float>(font.width);
	textCursor.h = static_cast<float>(font.height);

	RegisterCommands();

}

Terminal::~Terminal() {

	userinput.clear();
	history.clear();

	for (TextCache& texture : textCache) {
		if (texture.texture) {
			SDL_DestroyTexture(texture.texture);
		}
	}

	textCache.clear();

	TTF_CloseFont(font.ttf_font);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window.sdl_window);

	TTF_Quit();
	SDL_Quit();

}

void Terminal::Init() {

	SDL_Event e;

	SDL_StartTextInput(window.sdl_window);

	while (!window.shouldclose) {

		while (SDL_PollEvent(&e)) {
			HandleEvents(e);
		}
		
		if (window.shouldclose) {
			break;
		}

		ClearWindow();

		DrawContent();

		SDL_RenderPresent(renderer);

	}

	SDL_StopTextInput(window.sdl_window);

}

void Terminal::Print(const std::string& msg, SDL_Color text_color) {
	history.emplace_back(msg, text_color, bgcolor);
}

void Terminal::HandleEvents(SDL_Event& event) {

	switch (event.type) {
	
	case SDL_EVENT_QUIT:
		CloseWindow();
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		window.width = event.window.data1;
		window.height = event.window.data2;
		break;

	case SDL_EVENT_KEY_DOWN:
		KeyHandler(event.key);
		break;

	case SDL_EVENT_TEXT_INPUT:
		userinput += event.text.text;
		break;

	}

}

void Terminal::KeyHandler(SDL_KeyboardEvent& key) {

	switch (key.key) {

	case SDLK_ESCAPE:
		CloseWindow();
		break;

	case SDLK_BACKSPACE:
		if (!userinput.empty()) {
			userinput.pop_back();
		}
		break;

	case SDLK_RETURN:
		HandleInput();
		break;

	}

}

void Terminal::CloseWindow() {
	window.shouldclose = true;
}

void Terminal::ClearWindow() {
	SDL_SetRenderDrawColor(renderer, bgcolor.r, bgcolor.g, bgcolor.b, bgcolor.a);
	SDL_RenderClear(renderer);
}

void Terminal::UpdateTextCache(const std::vector<Text>& out) {

	for (size_t i = 0; i < out.size(); ++i) {
		if (i >= textCache.size() || textCache[i].data.text != out[i].text) {
			if (i >= textCache.size()) {
				textCache.push_back({ out[i], nullptr });
			}
			if (textCache[i].texture) {
				SDL_DestroyTexture(textCache[i].texture);
			}

			textCache[i].data = out[i];

			SDL_Surface* temp_surface = TTF_RenderText_Shaded(font.ttf_font, out[i].text.c_str(), out[i].text.size(), out[i].color, out[i].shadecolor);
			if (!temp_surface)
				continue;
			
			textCache[i].texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			if (!textCache[i].texture)
				continue;

			Vector2f size = { static_cast<float>(temp_surface->w), static_cast<float>(temp_surface->h) };
			textCache[i].size = size;

			SDL_DestroySurface(temp_surface);

		}
	}

	while (textCache.size() > out.size()) {
		SDL_DestroyTexture(textCache.back().texture);
		textCache.pop_back();
	}

}

void Terminal::FormatContent(std::vector<Text>& out) {

	size_t maxCharPerLine = window.width / font.width;

	for (Text& text : history) {
		out.emplace_back(text.text, text.color, text.shadecolor);
	}

	SDL_Color input_color = WHITE;
	out.emplace_back(currentdir.string() + "> " + userinput, input_color, bgcolor);

	std::vector<Text> treatedText = { };

	for (const Text& text : out) {

		size_t start = 0;

		while (start < text.text.size()) {
			
			size_t linebreak = text.text.find('\n', start);
			size_t length;

			if (linebreak != std::string::npos && linebreak - start <= maxCharPerLine) {
				length = linebreak - start;
			} else {
				length = std::min(maxCharPerLine, text.text.size() - start);
			}

			treatedText.emplace_back(text.text.substr(start, length), text.color, text.shadecolor);

			start += length;

			if (linebreak != std::string::npos && start == linebreak) {
				start++;
			}
		}

	}

	out = std::move(treatedText);
	treatedText.clear();
}

void Terminal::DrawContent() {

	Vector2 cursor = { 0, 0 };
	std::vector<Text> out = { };
	FormatContent(out);
	UpdateTextCache(out);

	int mw = 0;
	
	for (const TextCache& cached : textCache) {

		if (cached.data.text.empty()) {
			cursor.y += font.height;
			continue;
		}

		TTF_MeasureString(font.ttf_font, cached.data.text.c_str(), cached.data.text.size(), 0, &mw, nullptr);
		
		SDL_FRect frect = { 0.f, static_cast<float>(cursor.y), cached.size.x, cached.size.y };
		
		SDL_RenderTexture(renderer, cached.texture, nullptr, &frect);

		cursor.y += font.height;

	}

	if (mw >= window.width) {
		textCursor.x = 0.f;
		textCursor.y = static_cast<float>(cursor.y);
	} else {
		textCursor.x = static_cast<float>(mw);
		textCursor.y = static_cast<float>(cursor.y - font.height);
	}

	SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
	SDL_RenderFillRect(renderer, &textCursor);

}

void Terminal::HandleInput() {

	history.emplace_back(currentdir.string() + "> " + userinput + "\n", WHITE, bgcolor);

	std::vector<std::string> tokens = {};

	std::stringstream ss(userinput);
	std::string token;

	while (ss >> token) {
		tokens.push_back(token);
	}

	if (tokens.empty()) {
		return;
	}

	auto mapIter = commandlist.find(tokens[0]);
	if (mapIter != commandlist.end()) {
		mapIter->second(static_cast<int>(tokens.size()), tokens);
	}

	userinput.clear();

}

void Terminal::RegisterCommands() {
	commandlist["cd"] = [this](int argc, std::vector<std::string> argv) { COMMAND_CD(argc, argv); };
	commandlist["ls"] = [this](int argc, std::vector<std::string> argv) { COMMAND_LS(argc, argv); };
	commandlist["cls"] = [this](int argc, std::vector<std::string> argv) { COMMAND_CLS(argc, argv); };
}

