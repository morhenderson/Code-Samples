/*
    util.h
    Morgan Henderson, Spring 2022
    Defines some utility functions (& maps) for the MycDrop program
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
        if (inStr[i]==dlim) {
            string newStr;
            outVec.push_back(newStr);
        }
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

// Produces a range of unsigned integer values given start & stop values
vector<unsigned> range(const unsigned &start, const unsigned &stop) {
    vector<unsigned> outVec;
    for (unsigned i=start; i<stop; i++) {outVec.push_back(i);}
    return outVec;
}

// Maps a string from the mycological datafile to an integer value
vector<map<string,double>> datMaps = {
    {{"e",0},{"p",1}},
    {{"b",0},{"c",1},{"f",2},{"x",3},{"k",4},{"s",5}},
    {{"f",0},{"g",1},{"y",2},{"s",3}},
    {{"n",0},{"b",1},{"c",2},{"g",3},{"r",4}},
    {{"f",0},{"t",1}},
    {{"a",0},{"l",1},{"c",2},{"y",3},{"f",4},{"m",5},{"n",6},{"p",7},{"s",8}},
    {{"a",0},{"d",1},{"f",2},{"n",3}},
    {{"c",0},{"w",1},{"d",2}},
    {{"b",0},{"n",1}},
    {{"k",0},{"n",1},{"b",2},{"h",3},{"g",4},{"r",5},{"o",6},{"p",7},{"u",8},{"e",9},{"w",10},{"y",11}},
    {{"e",0},{"t",1}},
    {{"b",0},{"c",1},{"u",2},{"e",3},{"z",4},{"r",5},{"?",6}},
    {{"f",0},{"y",1},{"k",2},{"s",3}},
    {{"f",0},{"y",1},{"k",2},{"s",3}},
    {{"n",0},{"b",1},{"c",2},{"g",3},{"o",4},{"p",5},{"e",6},{"w",7},{"y",8}},
    {{"n",0},{"b",1},{"c",2},{"g",3},{"o",4},{"p",5},{"e",6},{"w",7},{"y",8}},
    {{"p",0},{"u",1}},
    {{"n",0},{"o",1},{"w",2},{"y",3}},
    {{"n",0},{"o",1},{"t",2}},
    {{"c",0},{"e",1},{"f",2},{"l",3},{"n",4},{"p",5},{"s",6},{"z",7}},
    {{"k",0},{"n",1},{"b",2},{"h",3},{"r",4},{"o",5},{"u",6},{"w",7},{"y",8}},
    {{"a",0},{"c",1},{"n",2},{"s",3},{"v",4},{"y",5}},
    {{"g",0},{"l",1},{"m",2},{"p",3},{"u",4},{"w",5},{"d",6}},
};