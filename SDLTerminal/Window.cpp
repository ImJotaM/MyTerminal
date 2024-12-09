#include "Window.h"

Window::Window() {

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	SDL_Rect screen = { };
	if (!SDL_GetDisplayBounds(1, &screen)) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	window.title = "Terminal";
	window.width = screen.w / 2;
	window.height = screen.h / 2;
	window.flags = SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE;

	sdl_window = SDL_CreateWindow(window.title, window.width, window.height, window.flags);
	if (!sdl_window) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	sdl_renderer = SDL_CreateRenderer(sdl_window, NULL);
	if (!sdl_renderer) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

}

Window::~Window() {
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
}

const bool Window::shouldClose() const {
	return window.shouldclose;
}

void Window::ChangeBackgroundColor(SDL_Color& color) {
	window.bgcolor = color;
}

void Window::Clear() {

	SDL_Color* c = &window.bgcolor;
	SDL_SetRenderDrawColor(sdl_renderer, c->r, c->g, c->b, c->a);
	SDL_RenderClear(sdl_renderer);

}

void Window::RenderPresent() {
	SDL_RenderPresent(sdl_renderer);
}

void Window::Close() {
	window.shouldclose = true;
}

SDL_Renderer* Window::GetRenderer() {
	return sdl_renderer;
}
