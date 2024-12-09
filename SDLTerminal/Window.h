#pragma once
#include "Types.h"

class Window {

public:

	Window();
	~Window();

	const bool shouldClose() const;
	void ChangeBackgroundColor(SDL_Color& color);
	void Clear();
	void RenderPresent();
	void Close();

	operator SDL_Window*() {
		return sdl_window;
	}

	SDL_Renderer* GetRenderer();

private:

	struct WindowData {

		const char* title = "";
		int width = 0;
		int height = 0;
		SDL_WindowFlags flags = 0;

		SDL_Color bgcolor = { 0x0, 0x0, 0x0, 0xff };

		bool shouldclose = false;

	} window;

	SDL_Window* sdl_window = nullptr;
	SDL_Renderer* sdl_renderer = nullptr;

};