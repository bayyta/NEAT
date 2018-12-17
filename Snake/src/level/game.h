#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <random>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "entity/piece.h"
#include "../misc/input.h"
#include "../misc/vec2.h"
#include "neat.h"

namespace level {

	// 0 - game, 1 - training, 2 - training visualized (change _speed for faster / slower evolution process)
#define TRAINING 1

	using namespace entity;
	using namespace math;
	using String = std::string;

	class Game
	{
	private:
		Input _input;
		SDL_Window* _window;
		SDL_Renderer* _renderer;
		SDL_Rect _background;
		TTF_Font* _font;
		SDL_Surface* _text;
		SDL_Texture* _fontTex;
		static const int MAP_SIZE;
		const int W_WIDTH, W_HEIGHT;
		static const int PIECE_SIZE;
	private:
		int _stepsWOFood;
		const float rewardFunction();
	private:
		int _score;
		bool _dead;
		float _timer;
		float _speed;
		int _nextTurn;
		std::vector<Piece*> _pieces;
		std::vector<std::array<int, 3>> _turns; // x-coord, y-coord, direction
		std::vector<Piece*> _map; // store pieces' locations on map
		Vec2<int> _food;
	private:
		bool initGraphics(const char* title);
		void init();
		void resetVars();
		void addFood();
	public:
		Game(const char* title, const int& width, const int& height);
		~Game();
		void restart(neat::Neat& neat, const int& genomeIndex);
		void update(const float& dt, const int& genomeIndex, neat::Neat& neat);
		void render();
		bool closed();
		const int& getScore();
	};




}