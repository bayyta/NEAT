#include "input.h"

Input::Input()
{
	//std::cout << "Input class initialized\n";
	closed = false;
}

bool Input::keys[65536]{};
std::vector<SDL_Scancode> Input::pressed_keys;
std::vector<unsigned int> Input::clicked_buttons;
int Input::mx;
int Input::my;
int Input::mb;

void Input::update() {
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			closed = true;
			break;
		case SDL_KEYDOWN:
			keys[event.key.keysym.scancode] = true;
			break;
		case SDL_KEYUP:
			keys[event.key.keysym.scancode] = false;
			if (!pressed_keys.empty() && std::find(pressed_keys.begin(), pressed_keys.end(), event.key.keysym.scancode) != pressed_keys.end()) {
				pressed_keys.erase(std::find(pressed_keys.begin(), pressed_keys.end(), event.key.keysym.scancode));
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			mb = event.button.button;
			break;
		case SDL_MOUSEBUTTONUP:
			mb = 0;
			if (!clicked_buttons.empty() && std::find(clicked_buttons.begin(), clicked_buttons.end(), event.button.button) != clicked_buttons.end()) {
				clicked_buttons.erase(std::find(clicked_buttons.begin(), clicked_buttons.end(), event.button.button));
			}
			break;
		case SDL_WINDOWEVENT:
			switch (event.window.event) {
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				// TODO
				break;
			}
			break;
		}
	}
	SDL_GetMouseState(&mx, &my);
}

const bool& Input::key_pressed(const SDL_Scancode& key) {
	return keys[key];
}

bool Input::key_typed(const SDL_Scancode& key) {
	if (!key_pressed(key)) return false;
	if (!pressed_keys.empty() && std::find(pressed_keys.begin(), pressed_keys.end(), key) != pressed_keys.end()) 
	{
		return false;
	}
	pressed_keys.push_back(key);
	return true;
}

bool Input::mouse_clicked(const int& button) {
	bool clicked = mb == button;
	if (!clicked) return false;
	if (!clicked_buttons.empty() && std::find(clicked_buttons.begin(), clicked_buttons.end(), button) != clicked_buttons.end()) {
		return false;
	}
	clicked_buttons.push_back(button);
	return true;
}

const int& Input::get_mouse_button() {
	return mb;
}

const int& Input::get_MX() {
	return mx;
}

const int& Input::get_MY() {
	return my;
}

const bool& Input::window_closed() {
	return closed;
}