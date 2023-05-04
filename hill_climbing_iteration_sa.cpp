//
// Created by JunHyuckWoo on 30/09/2018.
//

#include "hill_climbing.h"
#include <time.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>
#include <map>
#include <string>
#include <iterator>
#include <unordered_set>
#include <random>
#include <set>
#include <cassert>

#define DEBUG_OUT true

using namespace std;

static default_random_engine engine{ (unsigned int)time(nullptr) };
constexpr double initialTemperature = 100.0;
double temperature = initialTemperature;

size_t Clamp(size_t value, size_t min, size_t max)
{
    if (value < min)
    {
        return min;
    }

    if (value > max)
    {
        return max;
    }

    return value;
}

size_t GetRandomNumber(size_t upperLimit)
{
    static uniform_int_distribution<size_t> distribution{ 0, upperLimit };
    return distribution(engine);
}

double GetRandomRealNumber()
{
    static uniform_real_distribution<double> distribution{ 0, 1 };
    return distribution(engine);
}

double GetNextTemperatureRatio(double time)
{
    return 1.0 / (1.0 + exp(0.55 * time));
}

double sigmoid(double x)
{
    double denominator = 1.0 + exp(-x);
    return 1.0 / denominator;
}

chrono::time_point<chrono::system_clock> timerStartTime1;
/*
hill_climbing::hill_climbing(const vector<Coordinate>& coordinates)
{
    timerStartTime1 = chrono::system_clock::now();
    size_t greedyStartPoint = GetRandomNumber(coordinates.size() - 1);

    greedy_algorithm ga(coordinates, greedyStartPoint);
    initialLength = ga.algorithm();
    coordinateLengthMatrix = std::move(ga.GetCoordinateLengthMartix());
    path = std::move(ga.GetPath());
}*/

hill_climbing::hill_climbing(pair<double, Path> generation, vector<vector<double>> coordinateLength)
{
    timerStartTime1 = chrono::system_clock::now();

    initialLength = generation.first;
    coordinateLengthMatrix = std::move(coordinateLength);
    path = std::move(generation.second);
}

double hill_climbing::TwoOptSwap(size_t swapStartIndex, size_t swapEndIndex)
{
    size_t startIndexPointId = path[swapStartIndex].first;
    size_t beforeStartIndexPointId = 0;
    if (swapStartIndex == 0)
    {
        beforeStartIndexPointId = path.back().first; // Starting point
    }
    else
    {
        beforeStartIndexPointId = path[swapStartIndex - 1].first;
    }

    size_t endIndexPointId = path[swapEndIndex].first;
    size_t afterEndIndexPointId = path[swapEndIndex + 1].first;

    double disconnectedStartPointLength = coordinateLengthMatrix[beforeStartIndexPointId][startIndexPointId];
    double connectedStartPointLength = coordinateLengthMatrix[startIndexPointId][afterEndIndexPointId];

    double disconnectedEndPointLength = coordinateLengthMatrix[endIndexPointId][afterEndIndexPointId];
    double connectedEndPointLength = coordinateLengthMatrix[beforeStartIndexPointId][endIndexPointId];

    double deltaLength =
            (connectedStartPointLength + connectedEndPointLength) -
            (disconnectedStartPointLength + disconnectedEndPointLength);

    if (deltaLength < 0)
    {
        reverse(path.begin() + swapStartIndex, path.begin() + swapEndIndex + 1);
        return deltaLength;
    }

    return 0.0; // fail
}

double hill_climbing::algorithm()
{
    cout << "Iteration + SA algorithm" << endl;
    double totalTourLength = initialLength;
    size_t totalPathSize = path.size();

    decltype(timerStartTime1) timerEndTime = chrono::system_clock::now();
    chrono::duration<double> duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime1);

    vector<pair<size_t, size_t>> indexPairs;
    for (size_t start = 0; start < totalPathSize - 2; ++start)
    {
        for (size_t end = start + 1; end < totalPathSize - 1; ++end)
        {
            if (start != end)
            {
                indexPairs.emplace_back(start, end);
            }
        }
    }

    pair<double, decltype(path)> bestTour;
    size_t searchPairIndex = 0;

    bestTour.first = totalTourLength;
    bestTour.second = path;

    for (size_t iterationNumber = 0; duration.count() < 64.0; ++iterationNumber)
    {
        size_t swapStartIndex = 0;
        size_t swapEndIndex = 0;

        swapStartIndex = indexPairs[searchPairIndex].first;
        swapEndIndex = indexPairs[searchPairIndex].second;

        double deltaTourLength = TwoOptSwap(swapStartIndex, swapEndIndex);

        totalTourLength += deltaTourLength;

        ++searchPairIndex;

        if (searchPairIndex >= indexPairs.size())
        {
            static uniform_int_distribution<size_t> distribution{ 0, (size_t) (totalPathSize - 2) };

            size_t pathSwappingLeftIndex = 0;
            size_t pathSwappingRightIndex = 0;

            while (pathSwappingLeftIndex == pathSwappingRightIndex)
            {
                pathSwappingLeftIndex = distribution(engine);
                pathSwappingRightIndex = distribution(engine);
            }

            double randomNumber = GetRandomRealNumber();
            double probability = 1.0 / (1 + exp((bestTour.first - totalTourLength) / temperature));

            if (bestTour.first > totalTourLength || randomNumber > probability)
            {
                timerEndTime = chrono::system_clock::now();
                duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime1);

                // cout << "[" << duration.count() << "] " << "Run!! " << totalTourLength << " (-" << (bestTour.first - totalTourLength) << ")" << endl;
                bestTour.first = totalTourLength;
                bestTour.second = std::move(path);
            }

            auto newPath = bestTour.second;
            swap(newPath[pathSwappingLeftIndex], newPath[pathSwappingRightIndex]);

            path = std::move(newPath);
            totalTourLength = CalculateTotalTourLength();

            searchPairIndex = 0;
        }

        timerEndTime = chrono::system_clock::now();
        duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime1);

        double elapsedTime = duration.count();
        temperature = initialTemperature * GetNextTemperatureRatio(elapsedTime - 15.0);
    }

    cout << "Total elapsed time : " << duration.count() << " seconds" << endl;

    if (bestTour.first > totalTourLength)
    {
        cout << "Final Run!! " << totalTourLength << endl;
        bestTour.first = totalTourLength;
        bestTour.second = std::move(path);
    }

    cout << "best tour length : " << bestTour.first << endl;
    path = std::move(bestTour.second);
    // Assertion (For accuracy.)

    // Unique tour?
    vector<size_t> count;
    count.resize(totalPathSize);
    for (auto& pair : path)
    {
        ++count[pair.first];
    }

    for (auto& c : count)
    {
        assert(c == 1);
    }

    // Tour length correct?
    cout << "Please check your tour length is correct : " << CalculateTotalTourLength() << endl;

    // Assertion End

    /*************************** Write to file *********************************/
    ofstream fileWriter{ "output.txt", ios::out };

#ifdef DEBUG_OUT
    fileWriter << initialLength << endl << totalTourLength << endl;

    fileWriter << path.back().second.first << " "
        << path.back().second.second << endl;

    for (const auto& indexWithCoordinate : path)
    {
        fileWriter << indexWithCoordinate.second.first << " "
            << indexWithCoordinate.second.second << endl;
    }
#else
    fileWriter << path.back().first << endl;

    for (const auto& indexWithCoordinate : path)
    {
        fileWriter << indexWithCoordinate.first << endl;
    }
#endif

    fileWriter.close();
    /******************************* End **************************************/


    return totalTourLength;
}

double hill_climbing::CalculateTotalTourLength()
{
    double totalLength = 0.0;
    size_t startPoint = path.back().first;

    for (const auto& indexWithCoordinate : path)
    {
        totalLength += coordinateLengthMatrix[startPoint][indexWithCoordinate.first];
        startPoint = indexWithCoordinate.first;
    }

    return totalLength;
}