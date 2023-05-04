//
// Created by JunHyuckWoo on 16/10/2018.
//

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
#include "greedy_algorithm.h"
#include "genetic_algorithm.h"

chrono::time_point<chrono::system_clock> timerStartTime;

static default_random_engine engine{ (unsigned int) time(nullptr) };
//static default_random_engine engine{ 0 };

size_t roulette(double SumOfFitnesses, size_t numberOfParents, vector<double> fitness) {

    if(SumOfFitnesses == 0) {
        SumOfFitnesses = 1;
    }
    size_t point = rand()%(size_t)(SumOfFitnesses);
    double sum = 0 ;
    for (size_t i = 0; i < numberOfParents; i++) {
        sum = sum + fitness.at(i);
        if (point < sum) {
            return i;
        }
    }
    cout<<endl;
    return 0;
}

void printing(Path path) {
    for (int i = 0; i<path.size(); i++) {
        cout << path[i].first << " ";
    }
    cout << endl;
}

void printGen(vector<pair<double, Path>> generation) {
    for (int i = 0; i < generation.size(); i++) {
        cout << generation.at(i).first << " ";
    }
    cout << endl;
}

double genetic_algorithm::CalculateTotalTourLength(Path& path)
{
    double totalLength = 0.0 , buf = 0.0;
    wp = 0;
    size_t startPoint = path.back().first;

    for (const auto& indexWithCoordinate : path)
    {
        totalLength += coordinateLengthMatrix[startPoint][indexWithCoordinate.first];
        if(buf < coordinateLengthMatrix[startPoint][indexWithCoordinate.first]){
            wp = indexWithCoordinate.first;
            buf = coordinateLengthMatrix[startPoint][indexWithCoordinate.first];
        }
        startPoint = indexWithCoordinate.first;
    }

    return totalLength;
}

genetic_algorithm::genetic_algorithm(const vector<Coordinate>& coordinates, size_t numParents, double greedyParentRatio)
{
    timerStartTime = chrono::system_clock::now();
    
    numberOfParents = numParents;
    wp = 0;
    worstPath = 0; k = 4;
    bestPath = MAXFLOAT;
    uniform_int_distribution<size_t> distribution{ 0, coordinates.size() - 1 };
    
    greedy_algorithm greedy{ coordinates, 0 };
    size_t numGreedyParents = (size_t) (numParents * greedyParentRatio);

    if (numGreedyParents == 0)
    {
        numGreedyParents = 1;
    }

    for (size_t createdGreedyParent = 0; createdGreedyParent < numGreedyParents; ++createdGreedyParent)
    {
        greedy.Clear();

        size_t greedyStartPoint = distribution(engine);
        greedy.SetStartIndex(greedyStartPoint);

        greedyLength = greedy.algorithm();
        if(greedyLength > worstPath) {
            worstPath = greedyLength;
        }
        if(greedyLength < bestPath){
            bestPath = greedyLength;
        }
        generation.emplace_back(greedyLength, std::move(greedy.GetPath()));
    }

    coordinateLengthMatrix = std::move(greedy.GetCoordinateLengthMartix());

    for (size_t i = generation.size(); generation.size() < numParents; i++)
    {
        auto previousSibiling = generation[i - 1];
        shuffle(previousSibiling.second.begin(), previousSibiling.second.end(), engine);

        previousSibiling.first = CalculateTotalTourLength(previousSibiling.second);
        if(previousSibiling.first > worstPath) {
            worstPath = previousSibiling.first;
        }
        if(previousSibiling.first < bestPath){
            bestPath = previousSibiling.first;
        }
        generation.emplace_back(std::move(previousSibiling));
    }

#ifdef DEBUG_OUT
    cout << "======= Greedy Parents... '" << numGreedyParents << "' created ======" << endl;
    cout << "======= Purely Random Parents... '" << numParents - numGreedyParents << "' created ======" << endl;
#endif
}

Coordinate genetic_algorithm::getBest(vector<pair<size_t, Coordinate>> path) {
    Coordinate best;
    return best;
}



double genetic_algorithm::evaluate(vector<pair<size_t, Coordinate>> path) {
    double score = 1.0;
    double total = CalculateTotalTourLength(path);

    sort(generation.begin(), generation.end());
    for (int i = 0; i < generation.size(); i++)
    {
        if (generation.at(i).first < total)
            score++;
    }

    return score;
}


void genetic_algorithm::selection(vector<pair<double, Path>> generation, vector<pair<double, Path>> cross_generation) {

    generation.insert(generation.end(), cross_generation.begin(), cross_generation.end());
    //sort(generation.begin(), generation.end());

    //generation = cross_generation;

    /// Get fitness value to use roulette wheel
    for(size_t i=0; i<generation.size(); i++) {
        auto buf = (worstPath - generation.at(i).first) + (worstPath - bestPath)/k;
        fitness.push_back(buf);
        SumOfFitnesses +=buf;
    }

    if(SumOfFitnesses == 0){
        ofstream fileWriter{ "output.txt", ios::out };
        for(size_t i=0; i<131; i++){
            fileWriter<<generation.at(0).second.at(i).first << endl;
        }
        fileWriter.close();
        wp = 1;
    }

    /// Select number of parent from first
    for (int i=0; i < numberOfParents-1; i++) {
        auto buf = roulette(SumOfFitnesses, fitness.size(), fitness);
        next_generation.push_back(generation.at(buf));
    }
    fitness.clear();
    bestPath = MAXFLOAT;
    worstPath = 0;
    SumOfFitnesses = 0;

    sort(generation.begin(), generation.end());
    next_generation.push_back(generation.at(0));
}

pair<Path, Path> genetic_algorithm::crossOver(Path& p1, Path& p2) {

    size_t a = 0, b = 0, tmp;
    size_t len = p1.size();


    Path c1 = p1, c2 = p2;

    /// Select cutting point
    a = rand() % len;
    b = rand() % len;
    if (a == b && a != 0) { b = len - 1; }
    else if (a == b && a == 0) { a = len / 2; b = len - 1; }
    else if (a > b) {
        tmp = b;
        b = a;
        a = tmp;
    }
    //a = 0;

    /// Fill in the unindexed number
    for (size_t i = a; i <= b; i++) {
        for (size_t j = 0; j < len; j++) {
            if (c2.at(j) == p1.at(i)) {
                c2.erase(c2.begin() + j);
                break;
            }
        }
        for (size_t j = 0; j < len; j++) {
            if (c1.at(j) == p2.at(i)) {
                c1.erase(c1.begin() + j);
                break;
            }
        }
    }

    for (size_t i = 0; i < a; i++) {
        c1.emplace_back(c1.at(0));
        c2.emplace_back(c2.at(0));
        c1.erase(c1.begin());
        c2.erase(c2.begin());
    }

    for (size_t i = a; i < b + 1; i++) {
        c1.emplace(c1.begin() + i, p2.at(i));
        c2.emplace(c2.begin() + i, p1.at(i));
    }

    return pair<Path, Path>(c2, c1);
}

vector<pair<double, Path>> genetic_algorithm::create(vector<pair<double, Path>> path) {

    pair<Path, Path> buf;
    vector<pair<double, Path>> pathBuf;

    for(size_t i=0; i<path.size(); i++){
        if(path.at(i).first < bestPath) {
            bestPath = path.at(i).first;
        }
        if(path.at(i).first > worstPath) {
            worstPath = path.at(i).first;
        }
    }
    for(size_t i=0; i<generation.size(); i++) {
        auto buf3 = (worstPath - generation.at(i).first) + (worstPath - bestPath)/k;
        fitness.push_back(buf3);
        SumOfFitnesses +=buf3;
    }

    for (int i = 0; i < numberOfParents; i++) {
        auto b1 = roulette(SumOfFitnesses, fitness.size(), fitness);
        auto b2 = roulette(SumOfFitnesses, fitness.size(), fitness);
        buf = crossOver(path[b1].second, path[b2].second);
        double buf1 = CalculateTotalTourLength(buf.first);
        double buf2 = CalculateTotalTourLength(buf.second);
        pathBuf.emplace_back(pair<double, Path>(buf1, buf.first));
        pathBuf.emplace_back(pair<double, Path>(buf2, buf.second));
    }

    /// Get the
    for(size_t i=0; i<pathBuf.size(); i++){
        if(pathBuf.at(i).first < bestPath) {
            bestPath = pathBuf.at(i).first;
        }
        if(pathBuf.at(i).first > worstPath) {
            worstPath = pathBuf.at(i).first;
        }
    }

    fitness.clear();
    SumOfFitnesses = 0;

    return pathBuf;
}

double genetic_algorithm::algorithm() {

    size_t iterationNumber;

    decltype(timerStartTime) timerEndTime = chrono::system_clock::now();
    chrono::duration<double> duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime);

    cout << "First Gen : "; printGen(generation); //for test

    // Finish after 119 sec
    for (iterationNumber = 0; duration.count() < 45; ++iterationNumber) {

        selection(generation, create(generation));	// select from previous generation
        if(wp == 1) {
            return 0.0;
        }
        generation = next_generation;			    // make child gen. to parent gen.
        next_generation.clear();
        sort(generation.begin(), generation.end());
        cout << bestPath << " || " << iterationNumber<< "th Gen : "; printGen(generation); // for test

        timerEndTime = chrono::system_clock::now();
        duration = static_cast<chrono::duration<double>>(timerEndTime - timerStartTime);
        //	cout << "Time : " << duration.count() << endl; // For debugging
    }

    cout << "Final Generation : " << iterationNumber << endl;
    cout << "Best : " << generation.at(0).first << endl;
    cout << "Time : " << duration.count() << endl;


    /*************************** Write to file *********************************/



    //fileWriter << initialLength << endl << totalTourLength << endl;



#ifdef DEBUG_OUT
    //fileWriter << path.back().first << endl;
    ofstream fileWriter{ "output.txt", ios::out };
    for(size_t i=0; i<generation.at(0).second.size(); i++){
        fileWriter<<generation.at(0).second.at(i).first << endl;
    }
    fileWriter.close();
#endif


    /******************************* End **************************************/

    return 0.0;

}