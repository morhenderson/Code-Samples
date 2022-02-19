/*/ 
    main.cpp
    Morgan Henderson, Spring 2022
    A genetic algorithm for matching input phrases
/*/

// Include standard library headers & adopt the std namespace
#include <string>
#include <vector>
#include <random>
#include <time.h>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// Include local headers
#include "util.h"
#include "config.h"
#include "genome.h"

// Main program loop
int main(int argc, char* argv[]) {

    // Initialize a filepath string & a vector for split path strings
    string fpath;
    vector<string> path_items;

    // Construct the root path & the config file path
    split(argv[0],path_items,'/');
    path_items.pop_back();
    const string root = join(path_items,'/');
    const string configPath = root+"config";

    // Read the configuration file
    if (readConfig(configPath)) {
        cout << "WARNING: Could not read config, using default parameters.";
    }

    // Construct an initial pool of individuals with random genomes
    vector<genome> pool;
    vector<double> fitnesses;
    for (unsigned p=0; p<popSize; p++) {
        genome member;
        pool.push_back(member);
        fitnesses.push_back(member.fitness);
    }
    double maxFit = *max_element(fitnesses.begin(),fitnesses.end());

    // Propagate the gene pool until a perfect maximum fitness is achieved
    unsigned gen = 1;
    const unsigned nSurv = survRate*popSize;
    ofstream outFile(root+"output.txt");
    while (maxFit<1) {
        fitnesses.clear();

        // Print the details of this generation to the output file
        outFile << "GENERATION " << gen << "\n";
        for (unsigned p=0; p<popSize; p++) {
            outFile << "G" << gen << "-" << p << ": \t";
            outFile << "Quoth the raven, \"" << pool[p].genes << "\"\t";
            outFile << pool[p].fitness << "\n";
        }
        outFile << "MAXIMUM FITNESS: " << maxFit << "\n\n";
        
        // Sort the gene pool by fitness & cull, keeping top performers
        sort(pool.begin(),pool.end(),[](const genome& g1, const genome& g2) 
            {return g1.fitness>g2.fitness;});
        for (unsigned p=0; p<popSize-nSurv; p++) {pool.pop_back();}

        // Store old genes & compute relative fitnesses
        double poolFit = 0;
        vector<double> wFit;
        vector<string> oldGenes;
        for (unsigned p=0; p<pool.size(); p++) {
            poolFit+=pool[p].fitness;
            oldGenes.push_back(pool[p].genes);
        }
        for (unsigned p=0; p<pool.size(); p++) {
            wFit.push_back(pool[p].fitness/poolFit);
        }

        // Propagate old genese to create the next generation of genomes
        pool.clear();
        for (unsigned p=0; p<popSize; p++) {
            int rPar = rWID(wFit);
            const string parentStr = oldGenes[rPar];
            genome member(parentStr);
            pool.push_back(member);
            fitnesses.push_back(pool[p].fitness);
        }
        maxFit = *max_element(fitnesses.begin(),fitnesses.end());
        gen++;
    }

    // Print the details of the final generation to the output file
    outFile << "GENERATION " << gen << "\n";
    for (unsigned p=0; p<popSize; p++) {
        outFile << "G" << gen << "-" << p << ": \t";
        outFile << "Quoth the raven, \"" << pool[p].genes << "\"\t";
        outFile << pool[p].fitness << "\n";
    }
    outFile << "MAXIMUM FITNESS: " << maxFit;

    return 0;
}