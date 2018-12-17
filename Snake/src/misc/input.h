#pragma once

#include <iostream>
#include <SDL.h>
#include <vector>

class Input {
private:
	SDL_Event event;
	static bool keys[65536];
	static std::vector<SDL_Scancode> pressed_keys;
	static std::vector<unsigned int> clicked_buttons;
	static int mx, my, mb;
	bool closed;
public:
	Input();
	void update();
	static const bool& key_pressed(const SDL_Scancode& key);
	static bool key_typed(const SDL_Scancode& key);
	static bool mouse_clicked(const int& button);
	static const int& get_MX();
	static const int& get_MY();
	static const int& get_mouse_button();
	const bool& window_closed();
};