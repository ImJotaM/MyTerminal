#include "Window.h"

Window::Window() {
	
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		std::exit(EXIT_FAILURE);
	}

	if (!TTF_Init()) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		SDL_Quit();
		std::exit(EXIT_FAILURE);
	}

}

Window::~Window() {
	TTF_Quit();
	SDL_Quit();
}

void Window::Configure(const char* title, int width, int height, Vector2 position, SDL_WindowFlags flags) {
	
	window.title = title;
	window.width = width;
	window.height = height;
	window.position = position;
	window.flags = flags;

}

void Window::Create() {
	sdl_window = SDL_CreateWindow(window.title, window.width, window.height, window.flags);
	if (!sdl_window) {
		SDL_LogError(SDL_LOG_PRIORITY_ERROR, SDL_GetError());
		TTF_Quit();
		SDL_Quit();
		std::exit(EXIT_FAILURE);
	}
}

const bool Window::shouldClose() const {
	return window.shouldclose;
}

void Window::Clear(SDL_Renderer* renderer, SDL_Color color) {
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderClear(renderer);
}

void Window::Close() {
	window.shouldclose = true;
}

void Window::Destroy() {
	SDL_DestroyWindow(sdl_window);
}

void Window::SetSize(int width, int height) {
	SDL_SetWindowSize(sdl_window, width, height);
}

int Window::GetWidth() {
	return window.width;
}
