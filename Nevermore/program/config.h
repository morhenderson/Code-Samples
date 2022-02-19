/*
    config.h
    Morgan Henderson, Spring 2022
    Sets default configuration parameters & defines a function to read user-defined parameters
*/

// Set default configuration parameters
string phrase = " ";
unsigned popSize = 10;
double mutRate = 0.1;
double survRate = 0.1;

// Reads a configuration file
unsigned readConfig(const string &fpath) {

    // Declare file parsing variables
    string conLine;
    vector<string> lItems;

    // Define configuration options & open the config file
    vector<string> settings = {"phrase","popSize","mutRate","survRate"};
    ifstream configFile(fpath);

    // Read through config file & assign configuration options
    while (getline(configFile,conLine)) {
        strip(conLine);
        split(conLine,lItems,':');
        const string setting = lItems[0];
        const string option = lItems[1];
        if (setting==settings[0]) {phrase = option;}
        else if (setting==settings[1]) {popSize = stod(option);}
        else if (setting==settings[2]) {mutRate = stod(option);}
        else if (setting==settings[3]) {survRate = stod(option);}
        else {return 1;}
    }
    return 0;
}