//
// Created by JunHyuckWoo on 16/10/2018.
//

#ifndef ARTIFICIAL_INTELLIGENCE_GENETIC_ALGORITHM_H
#define ARTIFICIAL_INTELLIGENCE_GENETIC_ALGORITHM_H

#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;
using Coordinate = pair<double, double>;
using Path = vector<pair<size_t, Coordinate>>;

class genetic_algorithm {
private:
    double greedyLength;
	size_t numberOfParents, k, wp;
	double worstPath, bestPath, SumOfFitnesses;
	vector<double> fitness;
	vector<pair<double, Path>> next_generation;


    double CalculateTotalTourLength(Path& path);
	double evaluate(vector<pair<size_t, Coordinate>> path);
    Coordinate getBest(vector<pair<size_t, Coordinate>> path);
	void selection(vector<pair<double, Path>> generation, vector<pair<double, Path>> cross_generation);
	vector<pair<double, Path>> create(vector<pair<double, Path>> path);
	pair<Path, Path> crossOver(Path& p1, Path& p2);


public:
    vector<vector<double>> coordinateLengthMatrix;
    vector<pair<double, Path>> generation;
    double algorithm();
    genetic_algorithm(const vector<Coordinate>& coordinates, size_t numParents, double greedyParentRatio);
};


#endif //ARTIFICIAL_INTELLIGENCE_GENETIC_ALGORITHM_H
