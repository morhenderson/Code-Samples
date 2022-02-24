/*
    config.h
    Morgan Henderson, Spring 2022
    Sets default configuration parameters & defines a function to read user-defined parameters
*/

// Set default configuration parameters
vector<unsigned> topology = {1};
double alpha = 0.5;
double eta = 0.15;
double RASmooth = 3;
unsigned nDat = 1;
double datSplit = 1.0;
vector<unsigned> inIDs = range(0,22);

// Reads a configuration file
unsigned readConfig(const string &fpath) {

    // Declare file parsing variables
    string conLine;
    vector<string> lItems;
    vector<string> oItems;

    // Define configuration options & open the config file
    vector<string> settings = {"topology","alpha","eta","RASmooth","nDat","datSplit","inIDs"};
    ifstream configFile(fpath);

    // Read through config file & assign configuration options
    while (getline(configFile,conLine)) {
        strip(conLine);
        split(conLine,lItems,':');
        const string setting = lItems[0];
        const string option = lItems[1];
        if (setting==settings[0]) {
            split(option,oItems,',');
            for (unsigned i=0; i<oItems.size(); i++) {topology.push_back(stoi(oItems[i]));}
        }
        else if (setting==settings[1]) {alpha = stod(option);}
        else if (setting==settings[2]) {eta = stod(option);}
        else if (setting==settings[3]) {RASmooth = stod(option);}
        else if (setting==settings[4]) {nDat = stod(option);}
        else if (setting==settings[5]) {datSplit = stod(option);}
        else if (setting==settings[6]) {
            if (option!="all") {
                inIDs.clear();
                split(option,oItems,',');
                for (unsigned i=0; i<oItems.size(); i++) {inIDs.push_back(stoi(oItems[i]));}
            }
        }
        else {return 1;}
    }
    if (alpha*eta*RASmooth*topology.size()*nDat*datSplit*inIDs.size()==0) {return 1;}
    else {return 0;}
}