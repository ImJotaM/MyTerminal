#pragma once
#include "Types.h"

class Window {

public:

	Window();
	~Window();

	void Configure(const char* title, int width, int height, Vector2 position, SDL_WindowFlags flags);
	void Create();

	const bool shouldClose() const;
	void Clear(SDL_Renderer* renderer, SDL_Color color);
	void Close();
	void Destroy();

	void SetSize(int width, int height);

	int GetWidth();

	operator SDL_Window*() {
		return sdl_window;
	}

private:

	struct WindowData {

		const char* title = "";
		int width = 0;
		int height = 0;
		Vector2 position = { };
		SDL_WindowFlags flags = 0;

		bool shouldclose = false;

	} window;

	SDL_Window* sdl_window = nullptr;

};