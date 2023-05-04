//
// Created by  on 30/09/2018.
//

#include "greedy_algorithm.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <iterator>
#include <unordered_set>
#include <chrono>

bool IsNumericString(const string& str)
{
    try
    {
        stod(str);
        return true;
    }
    catch (const invalid_argument&)
    {
        return false;
    }
}

vector<Coordinate> ReadCoordinates(istream& readStream)
{
    vector<string> wordsInStream{ istream_iterator<string>{ readStream },
                                  istream_iterator<string>{} };

    auto coordStartPosition = find(wordsInStream.cbegin(), wordsInStream.cend(), "NODE_COORD_SECTION");

    vector<string> numbersInStream;
    copy_if(coordStartPosition + 1, wordsInStream.cend(), back_inserter(numbersInStream), IsNumericString);

    vector<Coordinate> coordinates;
    for (auto currentIterator = numbersInStream.cbegin();
         currentIterator != numbersInStream.cend();
         /* Empty */)
    {
        ++currentIterator; // throw index of coordinate

        double x = stod(*(currentIterator++));
        double y = stod(*(currentIterator++));

        coordinates.emplace_back(x, y);
    }

    return coordinates;
}

size_t greedy_algorithm::CoordinateSize()
{
    return coordinates.size();
}

greedy_algorithm::greedy_algorithm(const vector<Coordinate>& coordinates, size_t index)
    : coordinates(coordinates) 
{
    startIndex = index;
    
    coordinateLengthMatrix.resize(coordinates.size());
    for (auto& rowIter : coordinateLengthMatrix)
    {
        rowIter.resize(coordinates.size());
    }
}

pair<size_t, double> greedy_algorithm::FindNearestCoordinate(const vector<Coordinate>& coordinates, size_t currentCoordinateIndex,
                                                             unordered_set<size_t>& visitedList)
{
    auto minimumLength = numeric_limits<double>::max();
    size_t nearestCoordinateIndex = 0;

    for (size_t i = 0; i < coordinates.size(); ++i)
    {
        if (currentCoordinateIndex != i && visitedList.find(i) == visitedList.end())
        {
            if (visitedList.size() == coordinates.size() - 1 ||
                (visitedList.size() < coordinates.size() - 1 && i != startIndex))
            {
                double xDifference = coordinates[currentCoordinateIndex].first - coordinates[i].first;
                double yDifference = coordinates[currentCoordinateIndex].second - coordinates[i].second;

                auto coordinateDistance = sqrt(pow(xDifference, 2) + pow(yDifference, 2));
                
                // Caching Matrix
                coordinateLengthMatrix[currentCoordinateIndex][i] = coordinateDistance;
                coordinateLengthMatrix[i][currentCoordinateIndex] = coordinateDistance;

                if (coordinateDistance < minimumLength)
                {
                    nearestCoordinateIndex = i;
                    minimumLength = coordinateDistance;
                }
            }
        }
    }

    visitedList.insert(nearestCoordinateIndex);
    return { nearestCoordinateIndex, minimumLength };
}

vector<vector<double>> greedy_algorithm::GetCoordinateLengthMartix() {
	return coordinateLengthMatrix;
}

vector<pair<size_t, Coordinate>> greedy_algorithm::GetPath() {
	return path;
}

void greedy_algorithm::Clear()
{
    path = decltype(path){};
    visitedList.clear();
}

void greedy_algorithm::SetStartIndex(size_t newStartIndex)
{
    startIndex = newStartIndex;
}

double greedy_algorithm::algorithm()
{
    double totalLength = 0.0;
    size_t currentCoordinateIndex = startIndex;

    cout << "Greedy started from : " << startIndex << endl;
    auto timerStartTime = chrono::system_clock::now();

	while (visitedList.size() != coordinates.size())
	{
		auto result = FindNearestCoordinate(coordinates, currentCoordinateIndex, visitedList);
		currentCoordinateIndex = result.first;
		totalLength += result.second;
		
        path.emplace_back(currentCoordinateIndex, coordinates[currentCoordinateIndex]);
	}

	auto timerEndTime = chrono::system_clock::now();
	auto duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime);

	cout << "Greedy tour length : " << totalLength << endl;
	cout << "Greedy elapsed time : " << duration.count() << " seconds" << endl;
    cout << "-------------------------------" << endl;

#ifdef DEBUG_OUT
    vector<size_t> count(path.size(), 0);
    for (auto& pair : path)
    {
        ++count[pair.first];
    }

    for (auto& c : count)
    {
        if (c != 1)
        {
            throw invalid_argument{ "not unique" };
        }
    }
    
    if (CalculateTotalTourLength() != totalLength)
    {
        throw invalid_argument{ "not correct" };
    }

    /*************************** Write to file *********************************/
    ofstream fileWriter{ "greedy_output.txt", ios::out };

    fileWriter << "Started from : " << startIndex << endl
        << "Total greedy length : " << totalLength << endl
        << "Total elapsed time : " << duration.count() << " seconds" << endl;
    fileWriter.close();
    /******************************* End **************************************/
#endif

    return totalLength;
}

double greedy_algorithm::CalculateTotalTourLength()
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