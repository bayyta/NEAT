#include <iostream>
#include <memory>
#include "neat.h"
#include "network/genome/genome.h"
#include "timer.h"

int main()
{
	neat::Neat neat;
	// set neat values as needed
	neat.values.INPUTS = 2;
	neat.values.OUTPUTS = 1;
	neat.values.POPULATION = 200;
	neat.values.COMP_THRESHOLD = 5.0f;
	neat.values.RANDOM_RANGE = 3.0f;
	neat.values.SIGMA = 3.0f;
	neat.values.P_ADD_NODE = 0.003f;
	neat.values.P_ADD_CONNECTION = 1.00f;
	neat.values.P_MUTATE_WEIGHTS = 1.00f;

	// initialize
	neat.init();

	// game
	int patterns[4][2] = { {0,0},{1,0},{0,1},{1,1} };

	// neat stats
	int runs = 100;
	int failedRuns = 0;
	int maxGensPerRun = 200;
	int avgGens = 0;
	float avgConnections = 0;
	float avgNodes = 0.0f;
	int bestGens = 999999999;
	int bestConnections = 99999999;
	int bestNodes = 999999999;
	int bestGensCount = 1;
	int bestNodesCount = 1;
	int bestConnectionsCount = 1;

	// timer
	Timer _time;
	float timer = 0;
	float dt = 0.0f;
	float last = _time.elapsed();
	int ups = 0;

	for (int run = 0; run < runs; run++)
	{
		neat.restart();
		bool finished = false;
		while (!finished)
		{
			ups++;
			dt = _time.elapsed() - last;
			last = _time.elapsed();
			if (_time.elapsed() - timer >= 1.0f)
			{
				timer += 1.0f;
				printf("%d ups\n", ups);

				printf("%.1f %%\n", (float)(run) * 100.0f / runs);

				ups = 0;
			}

			float fitness = 4.0f;
			for (int genomeIndex = 0; genomeIndex < neat.getPopulationCount(); genomeIndex++)
			{
				int rightCount = 0;
				fitness = 4.0f;

				for (int i = 0; i < 4; i++)
				{
					neat.setInput(patterns[i][0], 0, genomeIndex);
					neat.setInput(patterns[i][1], 1, genomeIndex);

					neat.updateOutput(genomeIndex);

					const float diff = abs(neat.getOutput(0, genomeIndex) - (float)(patterns[i][0] ^ patterns[i][1]));

					if (diff < 0.5f)
					{
						rightCount++;
					}

					fitness -= diff;
				}

				fitness *= fitness;
				neat.setFitness(fitness, genomeIndex);

				if (rightCount == 4)
				{
					finished = true;
				}
			}

			neat.feedForward();

			if (neat.getGeneration() >= maxGensPerRun)
			{
				failedRuns++;
				break;
			}
		}

		if (finished)
		{
			avgGens += neat.getGeneration();
			avgNodes += neat.getFittestGenome()->getHiddenNodeCount();
			avgConnections += neat.getFittestGenome()->getConnectionCount();

			if (neat.getGeneration() < bestGens)
			{
				bestGens = neat.getGeneration();
				bestGensCount = 1;
			}
			else if (neat.getGeneration() == bestGens)
			{
				bestGensCount++;
			}
			if (neat.getFittestGenome()->getHiddenNodeCount() < bestNodes)
			{
				bestNodes = neat.getFittestGenome()->getHiddenNodeCount();
				bestNodesCount = 1;
			}
			else if (neat.getFittestGenome()->getHiddenNodeCount() == bestNodes)
			{
				bestNodesCount++;
			}
			if (neat.getFittestGenome()->getConnectionCount() < bestConnections)
			{
				bestConnections = neat.getFittestGenome()->getConnectionCount();
				bestConnectionsCount = 1;
			}
			else if (neat.getFittestGenome()->getConnectionCount() == bestConnections)
			{
				bestConnectionsCount++;
			}
		}

	}

	avgGens /= (runs - failedRuns);
	avgNodes /= (runs - failedRuns);
	avgConnections /= (runs - failedRuns);

	printf("Runs: %d\n", runs);
	printf("Avarage generations: %d\nAvarage hidden nodes: %.3f\nAvarage connections: %.3f\n",
		avgGens, avgNodes, avgConnections);
	printf("Best generation: %d\t-x%d\nLeast hidden nodes: %d\t-x%d\nLeast connections: %d\t-x%d\n",
		bestGens, bestGensCount, bestNodes, bestNodesCount, bestConnections, bestConnectionsCount);
	printf("Failed: %d\n", failedRuns);


	system("PAUSE");
	return 0;
}