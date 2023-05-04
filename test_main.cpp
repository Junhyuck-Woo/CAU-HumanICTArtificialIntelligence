#include <iostream>
#include <fstream>
#include <random>
#include <time.h>
#include <chrono>
#include "hill_climbing.h"
#include "greedy_algorithm.h"
#include "genetic_algorithm.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		cout << "Usage : [program_name] [input_file_name]" << endl;
		return -1;
	}

	ifstream fileReader{ argv[1], ios::in };
	if (fileReader.fail())
	{
		cout << "Cannot open the file." << endl;

		return -1;
	}
    
    auto coordinates = ReadCoordinates(fileReader);

    constexpr size_t numberOfParents = 6;
    genetic_algorithm ga{ coordinates, numberOfParents, 1.0 };
    ga.algorithm();

    hill_climbing hc(ga.generation.at(0), ga.coordinateLengthMatrix);
    hc.algorithm();

	return 0;
}