/*
    util.h
    Morgan Henderson, Spring 2022
    Defines some utility functions & variables for the Nevermore program
*/

// Identifies a character as blank space
struct isBlank {
	bool operator()(char c) {
        return (c==' '||c=='\n'||c=='\r'||c=='\t'||c=='\v'||c=='\f');
    }
};

// Strips a string of any whitespace characters
void strip(string &inStr) {
    inStr.erase(remove_if(inStr.begin(),inStr.end(),isBlank()),inStr.end());
}

// Splits a string into a vector of strings at each instance of a delimiter
void split(const string &inStr, vector<string> &outVec, const char &dlim) {
    outVec.clear();
    outVec.push_back("");
    for (unsigned i=0; i<inStr.length(); i++) {
        if (inStr[i]==dlim) {outVec.push_back("");}
        else {outVec.back()=outVec.back()+inStr[i];}
    }
}

// Joins a vector of strings with an optional delimiter
string join(const vector<string> &inVec, const char &dlim) {
    string outStr;
    for (unsigned s=0; s<inVec.size(); s++) {
        outStr = outStr+inVec[s]+dlim;
    }
    return outStr;
}

// Replaces all instances of a character in a string
void replace(string &inStr, const char &oldChar, const char &newChar) {
    for (unsigned i=0; i<inStr.length(); i++) {
        if (inStr[i]==oldChar) {inStr[i]=newChar;}
    }
}

// An alphabet string, including a space
string alph = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ "};

// Create generators for random characters, mutations, & weighted fitness draws
mt19937 rng(time(NULL));
uniform_int_distribution<int> rCharID(0,alph.size());
uniform_real_distribution<double> stdU(0,1);
int rWID(const vector<double> &weights) {
    discrete_distribution<> wDisc(weights.begin(),weights.end());
    return wDisc(rng);
}