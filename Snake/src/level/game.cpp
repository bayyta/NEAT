#include "game.h"

namespace level {

	const int Game::MAP_SIZE = 20;
	const int Game::PIECE_SIZE = 20;

	Game::Game(const char* title, const int& width, const int& height)
		: W_WIDTH(width), W_HEIGHT(height)
	{
#if TRAINING != 1
		initGraphics(title);
#endif
		init();
	}

	Game::~Game()
	{
		for (auto& p : _pieces)
			delete p;

#if TRAINING != 1
		SDL_DestroyWindow(_window);
		_window = NULL;

		SDL_DestroyRenderer(_renderer);
		_renderer = NULL;

		SDL_FreeSurface(_text);
		SDL_DestroyTexture(_fontTex);

		SDL_Quit();
#endif
	}

	bool Game::initGraphics(const char* title)
	{
		bool success = true;

		// sdl initialization
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			printf("SDL initialization fail: %s\n", SDL_GetError());
			success = false;
		}

		// font initialization
		if (TTF_Init() < 0)
		{
			printf("TTF font initialization fail: %s\n", SDL_GetError());
			success = false;
		}

		// window
		_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W_WIDTH, W_HEIGHT, SDL_WINDOW_SHOWN);
		SDL_ShowCursor(SDL_ENABLE);
		if (_window == NULL)
		{
			printf("Window fail: %s\n", SDL_GetError());
			success = false;
		}

		// renderer
		_renderer = SDL_CreateRenderer(_window, -1, NULL);
		SDL_SetRenderDrawColor(_renderer, 255, 0, 255, 255);

		// font
		_font = TTF_OpenFont("res/monofonto.ttf", 26);
		_text = TTF_RenderText_Solid(_font, "Hello", { 255, 255, 255 });
		_fontTex = SDL_CreateTextureFromSurface(_renderer, _text);

		return success;
	}

	void Game::init()
	{
		resetVars();

		// set background dimensions
		_background.x = (W_WIDTH - MAP_SIZE * PIECE_SIZE) / 2;
		_background.y = (W_HEIGHT - MAP_SIZE * PIECE_SIZE) / 2;
		_background.w = MAP_SIZE * PIECE_SIZE;
		_background.h = MAP_SIZE * PIECE_SIZE;

		// set map size
		_map.resize(MAP_SIZE * MAP_SIZE);

		// add head
		Piece* head = new Piece(Vec2<int>(MAP_SIZE / 2, MAP_SIZE / 2), _nextTurn);
		_pieces.push_back(head);
		_map[head->position.y * MAP_SIZE + head->position.x] = head;

		// add food
		addFood();
	}

	void Game::restart(neat::Neat& neat, const int& genomeIndex)
	{
		resetVars();

		// clear map
		for (auto& m : _map)
			m = nullptr;

		// clear pieces and add head
		_pieces.clear();
		Piece* head = new Piece(Vec2<int>(MAP_SIZE / 2, MAP_SIZE / 2), _nextTurn);
		_pieces.push_back(head);
		_map[head->position.y * MAP_SIZE + head->position.x] = head;

		// add new food
		addFood();

		// clear turns
		_turns.clear();

#if TRAINING == 2
		// update text
		_text = TTF_RenderText_Solid(_font, (String("Gen: ") + String(std::to_string(neat.getGeneration())) + 
			String(", Num: ") + std::to_string(genomeIndex)).c_str(), { 255, 255, 255 });
		_fontTex = SDL_CreateTextureFromSurface(_renderer, _text);
#endif
	}

	void Game::resetVars()
	{
		// reset game variables
		_stepsWOFood = 0;
		_timer = 0.0f;
		_speed = 100.0f;
		_nextTurn = 0;
		_score = 0;
		_dead = false;
	}

	void Game::update(const float& dt, const int& genomeIndex, neat::Neat& neat)
	{
		auto& head = _pieces[0];
		_timer += dt;
#if TRAINING == 0
		_input.update();

		if (Input::key_typed(SDL_SCANCODE_UP))
		{
			_nextTurn = 0;
		}
		if (Input::key_typed(SDL_SCANCODE_RIGHT))
		{
			_nextTurn = 1;
		}
		if (Input::key_typed(SDL_SCANCODE_DOWN))
		{
			_nextTurn = 2;
		}
		if (Input::key_typed(SDL_SCANCODE_LEFT))
		{
			_nextTurn = 3;
		}
#endif
#if TRAINING != 1
		// update snake
		if (_timer >= (1.0f / _speed))
		{
#endif
#if TRAINING != 0
			_stepsWOFood++;
			if (_stepsWOFood >= (MAP_SIZE * MAP_SIZE) / 4)
			{
				_dead = true;
				neat.setFitness(rewardFunction(), genomeIndex);
			}
			// set input - collision
			int xpos;
			int ypos;
			for (int inputIndex = 0; inputIndex < 3; inputIndex++)
			{
				switch (inputIndex)
				{
				case 0:
					// collision left?
					xpos = head->position.x + (head->direction - 1) % 2;
					ypos = head->position.y + (head->direction - 2) % 2;
					break;
				case 1:
					// collision forward?
					xpos = head->position.x + (-(head->direction) + 2) % 2;
					ypos = head->position.y + (head->direction - 1) % 2;
					break;
				case 2:
					// collision right?
					xpos = head->position.x + (-(head->direction) + 1) % 2;
					ypos = head->position.y + (-(head->direction) + 2) % 2;
					break;
				default:
					break;
				}
				if (ypos < 0 || ypos >= MAP_SIZE || xpos < 0 || xpos >= MAP_SIZE)
				{
					neat.setInput(0, inputIndex, genomeIndex);
				}
				else if (_map[ypos * MAP_SIZE + xpos] != nullptr)
				{
					neat.setInput(0, inputIndex, genomeIndex);
				}
				else
				{
					neat.setInput(1, inputIndex, genomeIndex);
				}
			}

			// set input - food direction
			bool foodLeft = 0;
			bool foodForward = 0;
			bool foodRight = 0;
			if (_food.x == head->position.x && _food.y < head->position.y)
			{
				// food north
				switch (head->direction)
				{
				case 0:
					foodForward = 1;
					break;
				case 1:
					foodLeft = 1;
					break;
				case 3:
					foodRight = 1;
					break;
				default:
					break;
				}
			}
			else if (_food.x == head->position.x && _food.y > head->position.y)
			{
				// food south
				switch (head->direction)
				{
				case 1:
					foodRight = 1;
					break;
				case 2:
					foodForward = 1;
					break;
				case 3:
					foodLeft = 1;
					break;
				default:
					break;
				}
			}
			else if (_food.x < head->position.x && _food.y == head->position.y)
			{
				// food west
				switch (head->direction)
				{
				case 0:
					foodLeft = 1;
					break;
				case 2:
					foodRight = 1;
					break;
				case 3:
					foodForward = 1;
					break;
				default:
					break;
				}
			}
			else if (_food.x > head->position.x && _food.y == head->position.y)
			{
				// food east
				switch (head->direction)
				{
				case 0:
					foodRight = 1;
					break;
				case 1:
					foodForward = 1;
					break;
				case 2:
					foodLeft = 1;
					break;
				default:
					break;
				}
			}
			// input index 3, 4, 5 - food left?, food forward?, food right?
			neat.setInput(foodLeft, 3, genomeIndex);
			neat.setInput(foodForward, 4, genomeIndex);
			neat.setInput(foodRight, 5, genomeIndex);

			// evaluate output
			neat.updateOutput(genomeIndex);

			// get output
			if (neat.getOutput(0, genomeIndex) >= neat.getOutput(1, genomeIndex) &&
				neat.getOutput(0, genomeIndex) >= neat.getOutput(2, genomeIndex))
			{
				// left
				_nextTurn = (head->direction + 3) % 4;
			}
			else if (neat.getOutput(2, genomeIndex) >= neat.getOutput(0, genomeIndex) &&
				neat.getOutput(2, genomeIndex) >= neat.getOutput(1, genomeIndex))
			{
				// right
				_nextTurn = (head->direction + 1) % 4;
			}
			else
			{
				// forward
				_nextTurn = (head->direction + 0) % 4;
			}
#endif
			// add new turn if needed
			if (_nextTurn != head->direction)
			{
				_turns.push_back({ head->position.x, head->position.y, _nextTurn });
			}
			// change direction of pieces according to turns
			for (auto t = _turns.begin(); t != _turns.end();)
			{
				auto& p = _map[(*t)[1] * MAP_SIZE + (*t)[0]];
				if (p == nullptr)
				{
					t = _turns.erase(t);
					continue;
				}

				p->direction = (*t)[2];

				t++;
			}
			// clear map
			for (auto& m : _map)
				m = nullptr;
			// update map with new positions of pieces
			for (int i = _pieces.size() - 1; i >= 1; i--)
			{
				auto& p = _pieces[i];
				p->move();
				_map[p->position.y * MAP_SIZE + p->position.x] = p;
			}
			// check if head collides
			head->move();

			if (_map[head->position.y * MAP_SIZE + head->position.x] != nullptr ||
				head->position.x < 0 || head->position.x >= MAP_SIZE ||
				head->position.y < 0 || head->position.y >= MAP_SIZE)
			{
				// collision
				_dead = true;
#if TRAINING != 0
				neat.setFitness(rewardFunction(), genomeIndex);
#endif
			}
			else
			{
				_map[head->position.y * MAP_SIZE + head->position.x] = head;
			}
			// check if snake eats food
			if (head->position.x == _food.x && head->position.y == _food.y)
			{
				_score++;
				_stepsWOFood = 0;
				auto& prevLastPiece = _pieces[_pieces.size() - 1];
				Piece* newPiece;
				switch (prevLastPiece->direction)
				{
				case 0:
					newPiece = new Piece(Vec2<int>(prevLastPiece->position.x, prevLastPiece->position.y + 1), 0);
					break;
				case 1:
					newPiece = new Piece(Vec2<int>(prevLastPiece->position.x - 1, prevLastPiece->position.y), 1);
					break;
				case 2:
					newPiece = new Piece(Vec2<int>(prevLastPiece->position.x, prevLastPiece->position.y - 1), 2);
					break;
				case 3:
					newPiece = new Piece(Vec2<int>(prevLastPiece->position.x + 1, prevLastPiece->position.y), 3);
					break;
				default:
					break;

				}
				_pieces.push_back(newPiece);
				_map[newPiece->position.y * MAP_SIZE + newPiece->position.x] = newPiece;
				addFood();
			}

			_timer -= (1.0f / _speed);
#if TRAINING != 1
		}
#endif
	}

	void Game::addFood()
	{
		int count = 0;
		bool* availablePositions = new bool[MAP_SIZE * MAP_SIZE]{};
		for (int y = 0; y < MAP_SIZE; y++)
		{
			for (int x = 0; x < MAP_SIZE; x++)
			{
				if (_map[y * MAP_SIZE + x] == nullptr)
				{
					availablePositions[y * MAP_SIZE + x] = true;
					count++;
				}
			}
		}

		int pos = rand() % count;
		count = 0;
		for (int y = 0; y < MAP_SIZE; y++)
		{
			for (int x = 0; x < MAP_SIZE; x++)
			{
				if (availablePositions[y * MAP_SIZE + x])
				{
					if (pos == count)
					{
						// add food at this position
						_food.x = x;
						_food.y = y;
						delete[] availablePositions;
						return;
					}
					count++;
				}
			}
		}
	}

	void Game::render()
	{
		SDL_SetRenderDrawColor(_renderer, 255, 0, 255, 255);
		SDL_RenderClear(_renderer);

		// render background
		SDL_SetRenderDrawColor(_renderer, 155, 155, 155, 255);
		SDL_RenderFillRect(_renderer, &_background);

		// render food
		SDL_SetRenderDrawColor(_renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(_renderer, &SDL_Rect({ _food.x * PIECE_SIZE + _background.x,
			_food.y * PIECE_SIZE + _background.y, PIECE_SIZE, PIECE_SIZE }));

		// render snake
		SDL_SetRenderDrawColor(_renderer, 55, 55, 55, 255);
		for (auto& p : _pieces)
			SDL_RenderFillRect(_renderer, &SDL_Rect({ p->position.x * PIECE_SIZE + _background.x,
			p->position.y * PIECE_SIZE + _background.y, PIECE_SIZE, PIECE_SIZE }));

#if TRAINING == 2
		// render text
		SDL_RenderCopy(_renderer, _fontTex, NULL, &SDL_Rect({ 10, 10, _text->w, _text->h }));
#endif

		SDL_RenderPresent(_renderer);
	}

	bool Game::closed()
	{
		return _dead || _input.window_closed();
	}

	const float Game::rewardFunction()
	{
		return _score;
	}

	const int& Game::getScore()
	{
		return _score;
	}


}
