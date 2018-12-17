#include <iostream>
#include <random>
#include <time.h>
#include "level\game.h"
#include "misc\timer.h"

int main(int argc, char* argv[])
{
#if TRAINING != 0
	std::cout << "Training" << std::endl;
#endif
	srand(time(NULL));

	level::Game game("Snake", 960, 540);

	Timer timer;
	int seconds = 0;
	float dt = 0.0f;
	float last = timer.elapsed();
	int frames = 0;

	neat::Neat neat;
	// set neat values as needed
	neat.values.INPUTS = 6;
	neat.values.OUTPUTS = 3;
	neat.values.POPULATION = 150;
	neat.values.P_ADD_NODE = 0.01f;
	neat.values.P_ADD_CONNECTION = 0.3f;
	neat.values.SIGMA = 0.5f;
	neat.values.RANDOM_RANGE = 2.0f;
	neat.values.PERCENTAGE_OF_BEST = 0.9f;

	// initialize
	neat.init();

	// neat stats
	int runs = 1;
	int gens = 50;
	float avgHighestFitness = 0.0f;
	int bestScore = 0;

#if TRAINING != 0
	for (int run = 0; run < runs; run++)
	{
		neat.restart();
		int g = 0;
		while (neat.getGeneration() < gens)
		{
			for (int genomeIndex = 0; genomeIndex < neat.getPopulationCount(); genomeIndex++)
			{
				game.restart(neat, genomeIndex);
				while (!game.closed())
				{
					game.update(dt, genomeIndex, neat);
#if TRAINING == 2
					game.render();
#endif
					frames++;
					dt = timer.elapsed() - last;
					last = timer.elapsed();
					if (timer.elapsed() - seconds >= 1.0f)
					{
						//printf("%d ups\n", frames);
						printf("%.1f %%\n", 100.0f * (float)(run * gens + g) / (runs * gens));
						seconds++;
						frames = 0;
					}
				}
				if (game.getScore() > bestScore)
					bestScore = game.getScore();
			}

			neat.feedForward();
			g++;
		}

		avgHighestFitness += neat.getFittestGenome()->getFitness();

	}

	avgHighestFitness /= runs;
	printf("Avarage best fitness: %.2f\n", avgHighestFitness);
	printf("Best score: %d\n", bestScore);

	system("PAUSE");
#else
	while (!game.closed())
	{
		game.update(dt, 0, neat);
		game.render();

		frames++;
		dt = timer.elapsed() - last;
		last = timer.elapsed();
		if (timer.elapsed() - seconds >= 1.0f)
		{
			printf("%d ups\n", frames);
			seconds++;
			frames = 0;
		}
	}
#endif

	return 0;
}


