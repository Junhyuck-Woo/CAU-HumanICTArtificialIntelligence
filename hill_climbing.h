//
// Created by JunHyuckWoo on 30/09/2018.
//

#ifndef ARTIFICIAL_INTELLIGENCE_HILL_CLIMING_H
#define ARTIFICIAL_INTELLIGENCE_HILL_CLIMING_H

#include <iostream>
#include <vector>
#include <unordered_set>

using namespace std;
using Coordinate = pair<double, double>;
using Path = vector<pair<size_t, Coordinate>>;

class hill_climbing
{
private:
    double initialLength;
	
    vector<pair<size_t, Coordinate>> path;
	vector<vector<double>> coordinateLengthMatrix;

    double TwoOptSwap(size_t swapStartIndex, size_t swapEndIndex);
    double CalculateTotalTourLength();
public:
    double algorithm();
    //hill_climbing(const vector<Coordinate>& coordinates);
	hill_climbing(pair<double, Path> generation, vector<vector<double>> coordinateLengthMatrix);
};


#endif //ARTIFICIAL_INTELLIGENCE_HILL_CLIMING_H
