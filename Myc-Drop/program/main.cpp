/*/ 
    Myc-Drop.cpp
    Morgan Henderson, Spring 2022
    A fully-connected deep neural network for classifying mycological samples
/*/

// Include standard library headers & adopt the std namespace
#include <string>
#include <vector>
#include <math.h>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// Include local headers
#include "util.h"
#include "config.h"
#include "neuron.h"
#include "network.h"

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
    const string outSuffix = argc>1 ? argv[1] : "-results";
    const unsigned trainFlag = argc>2 ? stoi(argv[2]) : 1;
    const unsigned testFlag = argc>3 ? stoi(argv[3]) : 1;
    const unsigned summaryFlag = argc>4 ? stoi(argv[4]) : 0;

    // Read the configuration file & declare output file variables
    if (readConfig(configPath)) {
        cout << "WARNING: Could not read config, using default parameters.";
    }
    ofstream trainFile;
    ofstream testFile;
    ofstream summaryFile;

    // Initialize the network
    network MycNet;

    // Finalize the number of training & testing samples
    const unsigned nTrain = datSplit*nDat;
    const unsigned nTest = nDat-nTrain;

    // Open data file & declare file parsing variables
    ifstream datFile(root+"agaricus-lepiota.data");
    string datLine;
    vector<string> lItems;    

    // Reading through data, train the network
    if (trainFlag) {trainFile.open(root+"results/training"+outSuffix+".txt");}
    for (unsigned p=0; p<nTrain; p++) {

        // Declare holding vectors for inputs, outputs, & results
        vector<double> inputs;
        vector<double> outputs;
        vector<double> results;    

        // Read & split up a line of data
        getline(datFile,datLine);
        split(datLine,lItems,',');        

        // Map line items to integer values & place in input/output vectors
        outputs.push_back(datMaps[0][lItems[0]]);
        for (unsigned i=0; i<inIDs.size(); i++) {
            inputs.push_back(datMaps[inIDs[i]+1][lItems[inIDs[i]+1]]);
        }

        // Propagate inputs & outputs through network
        MycNet.feed(inputs);
        MycNet.learn(outputs);
        MycNet.report(results);

        // Report the outcome of this training epoch
        if (trainFlag) {
            trainFile << "EPOCH " << p+1 << "\n";
            trainFile << "Inputs: ";
            for (unsigned i=0; i<inIDs.size(); i++) {trainFile << inputs[i] << " ";}
            trainFile << "\nTarget: " << outputs.back() << "\n";
            trainFile << "Output: " << results.back() << "\n";
            trainFile << "Recent Average Error: " << MycNet.get_RAE();
            if (p<nTrain-1) {trainFile <<"\n\n";}
        }
    }
    trainFile.close();

    // Reading through remaining data, test the network
    unsigned corr = 0;
    unsigned fpos = 0;
    unsigned fneg = 0;
    if (testFlag) {testFile.open(root+"results/testing"+outSuffix+".txt");}
    for (unsigned p=0; p<nTest; p++) {

        // Declare holding vectors for inputs, outputs, & results
        vector<double> inputs;
        vector<double> results;    

        // Read & split up a line of data
        getline(datFile,datLine);
        split(datLine,lItems,',');

        // Map line items to integer values & place in input/output vectors
        const double truth = datMaps[0][lItems[0]];
        for (unsigned i=0; i<inIDs.size(); i++) {
            inputs.push_back(datMaps[inIDs[i]+1][lItems[inIDs[i]+1]]);
        }

        // Propagate inputs through network
        MycNet.feed(inputs);
        MycNet.report(results);

        // Construct recommendation & track false positives/negatives
        string rec = results.back()>0.5 ? "drop" : "keep";
        if ((rec=="drop"&&truth==1)||(rec=="keep"&&truth==0)) {corr++;}
        else if (rec=="drop"&&truth==0) {fpos++;}
        else if (rec=="keep"&&truth==1) {fneg++;}

        // Report the outcome of this training epoch
        if (testFlag) {
            testFile << "TEST " << p+1 << "\n";
            testFile << "Inputs: ";
            for (unsigned i=0; i<inIDs.size(); i++) {testFile << inputs[i] << " ";}
            testFile << "\nTarget: " << truth << "\n";
            testFile << "Output: " << results.back() << "\n";
            testFile << "Recommendation: " << rec << " that myc(ological sample)!" << "\n\n";
        }
    }

    // Provide a summary of testing outcomes
    string summaryText;
    if (testFlag||summaryFlag) {
        summaryText = "TEST SUMMARY\n"
            "Correct: \t\t" + to_string(corr) + " \t(" + to_string(100*corr/nTest) + "%)\n"
            "False (+): \t\t" + to_string(fpos) + " \t(" + to_string(100*fpos/nTest) + "%)\n"
            "False (-): \t\t" + to_string(fneg) + " \t(" + to_string(100*fneg/nTest) + "%)";
    }
    if (testFlag) {
        testFile << summaryText;
        testFile.close();
    }
    if (summaryFlag) {
        summaryFile.open(root+"results/summary"+outSuffix+".txt");
        summaryFile << summaryText;
        testFile.close();
    }
    return 0;
}