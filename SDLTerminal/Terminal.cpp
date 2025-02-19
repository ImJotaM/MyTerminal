#include "Terminal.h"

Terminal::Terminal() {

	terminal_mode = TERMINAL;
	SDL_Log("INFO: Current terminal mode -> [TERMINAL]");
	SDL_Log("");

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
			
		}

	}

	font.fontsize = 14 * display.scale;
	font.ttf_font = TTF_OpenFont("resources/fonts/Consola.ttf", font.fontsize);

	SDL_Log("");
	if (!font.ttf_font) {

		SDL_Log("STATUS: Default font (Consola) -> [ERROR]");
		SDL_LogError(SDL_LOG_PRIORITY_INVALID, SDL_GetError());
		SDL_Log("");

	} else {

		SDL_Surface* tmp_font_surface = TTF_RenderGlyph_Shaded(font.ttf_font, ' ', BLACK, BLACK);
		font.width = tmp_font_surface->w;
		font.height = tmp_font_surface->h;
		SDL_DestroySurface(tmp_font_surface);

		SDL_Log("STATUS: Default font (Consola) -> [OK]");
		SDL_Log("INFO: Using font size (Consola) -> [%.2fpx]", font.fontsize);
		SDL_Log("INFO: Using font dimensions (Consola) -> [%d x %d]", font.width, font.height);
		SDL_Log("");

	}

	window.sdl_window = SDL_CreateWindow(window.title, window.width, window.height, window.flags);
	ResizeWindow(window.width, window.height);

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
		SDL_Log("");
	}

	currentdir = fs::current_path();
	SDL_Log("INFO: Terminal starter directory -> %s", currentdir.string().c_str());

	cursor.frect.w = static_cast<float>(font.width);
	cursor.frect.h = static_cast<float>(font.height);
	SDL_Log("INFO: Text cursor dimensions -> [%.1f x %.1f]", cursor.frect.w, cursor.frect.h);

	scroll.scrollStep = font.height;
	SDL_Log("INFO: Scroll step -> %d", scroll.scrollStep);

	UpdateCurrentTime();
	cursor.lastBlinkTime = currentTime;
	SDL_Log("INFO: Starter terminal time -> %d ms", static_cast<int>(currentTime));

	RegisterCommands();
	SDL_Log("INFO: Registered commands: ");
	SDL_Log("");
	for (const auto& [key, value] : commandlist) {
		SDL_Log(" - %s", key.c_str());
	}

	UpdateViewport();
	
	UpdateContent();

	SDL_Log("");

}

Terminal::~Terminal() {

	userinput.clear();
	content.clear();

	for (TextCache& texture : textCache) {
		if (texture.texture) {
			SDL_DestroyTexture(texture.texture);
			texture.texture = nullptr;
		}
	}
	textCache.clear();

	TTF_CloseFont(font.ttf_font);
	font.ttf_font = nullptr;
	
	SDL_DestroyRenderer(renderer);
	renderer = nullptr;
	SDL_DestroyWindow(window.sdl_window);
	window.sdl_window = nullptr;

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

		UpdateCurrentTime();

		ClearWindow();

		DrawContent();

		SDL_RenderPresent(renderer);

	}

	SDL_StopTextInput(window.sdl_window);

}

void Terminal::Print(const std::string& msg, SDL_Color text_color) {
	content.emplace_back(msg, text_color, window.bgcolor);
}

void Terminal::UpdateCurrentTime() {
	currentTime = SDL_GetTicks();
}

void Terminal::HandleEvents(SDL_Event& event) {

	switch (event.type) {
	
	case SDL_EVENT_QUIT:
		CloseWindow();
		break;

	case SDL_EVENT_WINDOW_RESIZED:
		ResizeWindow(static_cast<int>(event.window.data1), static_cast<int>(event.window.data2));
		break;

	case SDL_EVENT_KEY_DOWN:
		KeyHandler(event.key);
		break;

	case SDL_EVENT_TEXT_INPUT:
		TextInputHandler(event.text);
		break;

	case SDL_EVENT_MOUSE_WHEEL:
		
		scroll.scrollOffset += (event.wheel.y > 0 ? scroll.scrollStep : -scroll.scrollStep);
		scroll.scrollOffset = std::min(0, scroll.scrollOffset);
		
		UpdateCursorPosition();
		
		break;
	}

}

void Terminal::KeyHandler(SDL_KeyboardEvent& key) {

	if (terminal_mode == TERMINAL) {

		switch (key.key) {

		case SDLK_ESCAPE:

			CloseWindow();
			break;

		case SDLK_BACKSPACE:

			if (!userinput.empty()) {
				userinput.pop_back();
			}
		
			cursor.Reset();
			UpdateContent();

			break;

		case SDLK_RETURN:

			HandleInput();
			break;

		}

	} else if (terminal_mode == EDITOR) {

		switch (key.key) {
		
		case SDLK_ESCAPE:
			
			ExitEditorMode();
			break;

		case SDLK_RIGHT:

			cursor.Reset();
			cursor.frect.x += font.width;
			break;

		case SDLK_LEFT:

			cursor.Reset();
			cursor.frect.x -= font.width;
			break;

		case SDLK_UP:

			cursor.Reset();
			cursor.frect.y -= font.height;
			break;

		case SDLK_DOWN:

			cursor.Reset();
			cursor.frect.y += font.height;
			break;

		}
		
	}

}

void Terminal::TextInputHandler(SDL_TextInputEvent& text) {

	cursor.Reset();

	if (terminal_mode == TERMINAL) {

		userinput += text.text;
		
		UpdateContent();

	} else if (terminal_mode == EDITOR) {

		UpdateContent();

	}

}

void Terminal::CloseWindow() {
	window.shouldclose = true;
}

void Terminal::ClearWindow() {
	SDL_SetRenderDrawColor(renderer, window.bgcolor.r, window.bgcolor.g, window.bgcolor.b, window.bgcolor.a);
	SDL_RenderClear(renderer);
}

void Terminal::ResizeWindow(int win_width, int win_height) {
	
	int width = static_cast<int>(win_width);
	int height = static_cast<int>(win_height);

	if (!(width % font.width == 0)) {
		width = (width / font.width) * font.width + font.width;
	}

	if (!(height % font.height == 0)) {
		height = (height / font.height) * font.height + font.height;
	}

	SDL_SetWindowSize(window.sdl_window, width, height);
	window.width = width;
	window.height = height;

	UpdateViewport();

}

void Terminal::UpdateContent() {

	out.clear();

	if (terminal_mode == TERMINAL) {
		TerminalFormatContent();
	} else if (terminal_mode == EDITOR) {
		EditorFormatContent();
	}

	UpdateTextCache();
	UpdateCursorPosition();
	UpdateCursorFocus();

}

void Terminal::TerminalFormatContent() {

	size_t maxCharPerLine = window.width / font.width;

	for (Text& text : content) {
		out.emplace_back(text.text, text.color, text.shadecolor);
	}

	SDL_Color input_color = WHITE;
	out.emplace_back(currentdir.string() + "> " + userinput, input_color, window.bgcolor);

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

void Terminal::EditorFormatContent() {

	for (Text& text : fileContent) {
		out.emplace_back(text.text, text.color, text.shadecolor);
	}

	std::vector<Text> treatedText = { };

	for (const Text& text : out) {

		size_t start = 0;

		if (text.text.empty()) {
			treatedText.emplace_back(text.text, text.color, text.shadecolor);
			continue;
		}

		while (start < text.text.size()) {

			size_t linebreak = text.text.find('\n', start);
			size_t length;

			if (linebreak != std::string::npos) {
				length = linebreak - start;
			} else {
				length = text.text.size() - start;
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

void Terminal::UpdateTextCache() {

	for (size_t i = 0; i < out.size(); i++) {
		if (i >= textCache.size() || textCache[i].data.text != out[i].text) {
			if (i >= textCache.size()) {
				textCache.push_back({ out[i], nullptr });
			}
			if (textCache[i].texture) {
				SDL_DestroyTexture(textCache[i].texture);
			}

			textCache[i].data = out[i];

			SDL_Surface* temp_surface = TTF_RenderText_Shaded(font.ttf_font, out[i].text.c_str(), out[i].text.size(), out[i].color, out[i].shadecolor);
			if (!temp_surface) {
				continue;
			}

			Vector2f size = { static_cast<float>(temp_surface->w), static_cast<float>(temp_surface->h) };
			textCache[i].size = size;

			textCache[i].texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
			SDL_DestroySurface(temp_surface);

			if (!textCache[i].texture) {
				continue;
			}
		}
	}

	while (textCache.size() > out.size()) {
		SDL_DestroyTexture(textCache.back().texture);
		textCache.pop_back();
	}

}

void Terminal::DrawContent() {

	drawcursor = { 0, scroll.scrollOffset };

	for (size_t i = 0; i < textCache.size(); i++) {

		const TextCache& cached = textCache[i];

		if (cached.data.text.empty()) {
			drawcursor.y += font.height;
			continue;
		}

		if (drawcursor.y + font.height >= viewport.min_y && drawcursor.y < viewport.max_y) {

			SDL_FRect frect = { 0.f, static_cast<float>(drawcursor.y), cached.size.x, cached.size.y };
			SDL_RenderTexture(renderer, cached.texture, nullptr, &frect);

		}

		drawcursor.y += font.height;
	}

	if (currentTime - cursor.lastBlinkTime >= cursor.blinkInterval) {
		cursor.cursorVisible = !cursor.cursorVisible;
		cursor.lastBlinkTime = currentTime;
	}

	if (cursor.cursorVisible) {
		SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
		SDL_RenderFillRect(renderer, &cursor.frect);
	}

}

void Terminal::HandleInput() {

	content.emplace_back(currentdir.string() + "> " + userinput + "\n", WHITE, window.bgcolor);

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

	UpdateContent();

}

void Terminal::UpdateCursorFocus() {

	if (terminal_mode == TERMINAL) {

		if (!IsCursorVisible()) {
			scroll.scrollOffset = std::min(0, -static_cast<int>(textCache.size() * font.height - viewport.max_y));
		}

	}

}

bool Terminal::IsCursorVisible() {
	return cursor.frect.y >= viewport.min_y && cursor.frect.y < viewport.max_y - font.height;
}

void Terminal::UpdateCursorPosition() {

	if (terminal_mode == TERMINAL) {

		int mw = 0;
		TTF_MeasureString(font.ttf_font, textCache.back().data.text.c_str(), textCache.back().data.text.size(), 0, &mw, nullptr);

		if (mw >= window.width) {
			cursor.frect.x = 0.f;
			cursor.frect.y = static_cast<float>(textCache.size() * font.height + scroll.scrollOffset);
		} else {
			cursor.frect.x = static_cast<float>(mw);
			cursor.frect.y = static_cast<float>(textCache.size() * font.height + scroll.scrollOffset - font.height);
		}

	} else if (terminal_mode == EDITOR){



	}

}

void Terminal::UpdateViewport() {

	viewport.min_x = 0;
	viewport.min_y = 0;
	viewport.max_x = window.width;
	viewport.max_y = window.height;

}

void Terminal::RegisterCommands() {

	commandlist["cd"]      = [this](int argc, std::vector<std::string> argv) { COMMAND_CD      (argc, argv); };
	commandlist["ls"]      = [this](int argc, std::vector<std::string> argv) { COMMAND_LS      (argc, argv); };
	commandlist["cls"]     = [this](int argc, std::vector<std::string> argv) { COMMAND_CLS     (argc, argv); };
	commandlist["mkdir"]   = [this](int argc, std::vector<std::string> argv) { COMMAND_MKDIR   (argc, argv); };
	commandlist["rm"]      = [this](int argc, std::vector<std::string> argv) { COMMAND_RM      (argc, argv); };
	commandlist["mkfile"]  = [this](int argc, std::vector<std::string> argv) { COMMAND_MKFILE  (argc, argv); };
	commandlist["run"]     = [this](int argc, std::vector<std::string> argv) { COMMAND_RUN     (argc, argv); };
	commandlist["edit"]    = [this](int argc, std::vector<std::string> argv) { COMMAND_EDIT    (argc, argv); };

}

void Terminal::StartEditorMode(fs::path& filedir) {

	SDL_Log("INFO: Current terminal mode -> [EDITOR]");
	SDL_Log("");

	terminal_mode = EDITOR;

	currentFile.open(filedir, std::ios::in | std::ios::out);

	if (!currentFile) {
		ExitEditorMode();
	}

	std::string fline = "";
	while (std::getline(currentFile, fline)) {
		fileBuffer.push_back(fline);
	}

	for (const std::string& line : fileBuffer) {
		fileContent.emplace_back(line, WHITE, window.bgcolor);
	}

	cursor.frect.x = 0.f;
	cursor.frect.y = 0.f;

	UpdateContent();

}

void Terminal::ExitEditorMode() {

	if (currentFile) {
		currentFile.close();
	}

	terminal_mode = TERMINAL;
	SDL_Log("INFO: Current terminal mode -> [TERMINAL]");
	SDL_Log("");

	UpdateContent();

}
