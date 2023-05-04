//
// Created by JunHyuckWoo on 30/09/2018.
//

#ifndef ARTIFICIAL_INTELLIGENCE_GREEDY_ALGORITHM_H
#define ARTIFICIAL_INTELLIGENCE_GREEDY_ALGORITHM_H

#define DEBUG_OUT

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_set>

using namespace std;
using Coordinate = pair<double, double>;

vector<Coordinate> ReadCoordinates(istream& readStream);

class greedy_algorithm {
private:
    vector<Coordinate> coordinates;
    vector<vector<double>> coordinateLengthMatrix;

    unordered_set<size_t> visitedList;
	vector<pair<size_t, Coordinate>> path;

    size_t startIndex;
    size_t currentCoordinateIndex;
private:
    pair<size_t, double> FindNearestCoordinate(const vector<Coordinate>& coordinates, size_t currentCoordinateIndex,
                                               unordered_set<size_t>& visitedList);
public:
    greedy_algorithm(const vector<Coordinate>& coordinates, size_t startIndex);
    double algorithm();
    void Clear();
    void SetStartIndex(size_t newStartIndex);
	vector<pair<size_t, Coordinate>> GetPath();
	vector<vector<double>> GetCoordinateLengthMartix();

    double CalculateTotalTourLength();
    size_t CoordinateSize();
};

#endif //ARTIFICIAL_INTELLIGENCE_GREEDY_ALGORITHM_H
