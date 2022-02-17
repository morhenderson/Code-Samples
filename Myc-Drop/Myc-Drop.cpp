/*/ 
    Myc-Drop.cpp
    Morgan Henderson, Spring 2022
    A fully-connected deep neural network for classifying mycological samples
/*/

#include <string>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
using namespace std;

// Splits a string into a vector of strings at each instance of a delimiter
vector<string> split(const string &inStr, const char &dlim) {
    vector<string> outVec;
    vector<unsigned> inds = {0,0};
    for (unsigned i=0; i<=inStr.length(); i++) {
        if (inStr[i]==dlim||i==inStr.length()) {
            inds[1] = i;
            string subStr = "";  
            subStr.append(inStr,inds[0],inds[1]-inds[0]);  
            outVec.push_back(subStr);  
            inds[0] = inds[1]+1;  
        }
    }
    return outVec;
}

// An edge between neurons in different layers of a network
struct edge {
    double weight;
    double delta;
};

// Declare the neuron class & create a typedef for a layer of neurons
class neuron;
typedef vector<neuron> layer;

// A neuron within a neural network
class neuron {
    public:
        neuron(const unsigned &nout, const unsigned &index);

        // Get & set functions for the neuron's output
        void set_out(const double &output) {_out = output;};
        double get_out(void) const {return _out;}

        // Computes gradients for output & hidden layers
        void outGrad(const double &output);
        void hidGrad(const layer &nlayer);

        // Feeds input into the neuron from the previous layer
        void feed(const layer &llayer);

        // Updates the 
        void update(layer &llayer, const double &alpha, const double &eta);

    private:

        // The neuron's index in its layer & edges
        unsigned _index;
        vector<edge> _edges;

        // The neuron's output & gradient
        double _out;
        double _grad;

        // Assigns a random weight to an edge
        static double _randweight() {return rand()/double(RAND_MAX);}
        
        // The neuron's activation function & its derivative
        static double _activate(const double &input);
        static double _dactivate(const double &input);

        // Sums the derivative of weights from the following layer
        double _sumDOWs(const layer &nlayer) const;
};

// Initializes an instance of the neuron class
neuron::neuron(const unsigned &nout, const unsigned &index) {
    for (unsigned e=0; e<nout; e++) {
        _edges.push_back(edge());
        _edges.back().weight = _randweight();
        _edges.back().delta = 0;
    }
    _index = index;
}

// Feeds inputs to a neuron from the preceding layer
void neuron::feed(const layer &llayer) {
    double sum = 0.;
    for (unsigned n=0; n<llayer.size(); n++) {
        sum += llayer[n].get_out()*llayer[n]._edges[_index].weight;
    }
    _out = _activate(sum);
}

// Computes gradients for the output layer
void neuron::outGrad(const double &output) {
    double delta = output-_out;
    _grad = delta*_dactivate(_out);
}

// Computes gradients for hidden layers
void neuron::hidGrad(const layer &nlayer) {
    double DOW = _sumDOWs(nlayer);
    _grad = DOW*_dactivate(_out);
}

// The activation function
double neuron::_activate(const double &input) {
    return tanh(input);
}

// The derivative of the activation function
double neuron::_dactivate(const double &input) {
    return 1-pow(tanh(input),2);
}

// Sums the derivative of weights in the following layer
double neuron::_sumDOWs(const layer &nlayer) const {
    double sum = 0.;
    for (unsigned n=0; n<nlayer.size()-1; n++) {
        sum += _edges[n].weight*nlayer[n]._grad;
    }
    return sum;
}

// Updates the previous layer weights associated with a neuron
void neuron::update(layer &llayer,const double &alpha, const double &eta) {
    for (unsigned n=0; n<llayer.size(); n++) {
        neuron &neighbor = llayer[n];
        double deltaOld = neighbor._edges[_index].delta;
        double delta = eta*neighbor.get_out()*_grad+alpha*deltaOld;
        neighbor._edges[_index].delta = delta;
        neighbor._edges[_index].weight += delta;
    }
}

// A neural network
class network {

    public:

        // Constructs an instance of the network class
        network(const string &fpath);

        // Feeds inputs into the network
        void feed(const vector<double> &inputs);

        // Performs back-propagation for the network
        void learn(const vector<double> &outputs);

        // Reports the network's outputs & recent average error
        void report(vector<double> &results) const;
        double get_RAE(void) {return _RAE;}

    private:

        // The network's topology & its layers of neurons
        vector<unsigned> _topology;
        vector<layer> _layers;

        // The network's learning rate & intertia
        double _eta;
        double _alpha;

        // The network's root mean square & recent average error (& smoothing factor)
        double _RMSE;
        double _RAE;
        double _RASmooth;

        // Reads a network configuration file
        unsigned _readConfig(const string &fpath);
};

// Initializes an instance of the network class
network::network(const string &fpath) {

    // Read the network config file
    if (_readConfig(fpath)) {
        cout << "WARNING: Config file could not be read, using default parameters.";
        _alpha = 0.5;
        _eta = 0.15;
        _RASmooth = 3;
        _topology={1,1};
    }

    // Construct the network layer by layer
    for (unsigned l=0; l<_topology.size(); l++) {
        _layers.push_back(layer());
        unsigned nout = l==_topology.size()-1 ? 0 : _topology[l+1];
        for (unsigned n=0; n<=_topology[l]; n++) {
            _layers.back().push_back(neuron(nout,n));
        }
        _layers.back().back().set_out(1.0);
    }

    // Initialize the network root mean square & recent average errors
    _RMSE = 0;
    _RAE = 0;
}

// Reads a network configuration file
unsigned network::_readConfig(const string &fpath) {

    // Declare file parsing variables
    string conLine;
    vector<string> lineItems;
    vector<string> itemItems;

    // Define configuration options & open the config file
    vector<string> conOps = {"alpha","eta","RASmooth","topology"};
    ifstream configFile(fpath);

    // Read through config file & assign configuration options
    while (getline(configFile,conLine)) {
        lineItems = split(conLine,':');
        if (lineItems[0]==conOps[0]) {_alpha = stod(lineItems[1]);}
        else if (lineItems[0]==conOps[1]) {_eta = stod(lineItems[1]);}
        else if (lineItems[0]==conOps[2]) {_RASmooth = stod(lineItems[1]);}
        else if (lineItems[0]==conOps[3]) {
            itemItems = split(lineItems[1],',');
            for (unsigned i=0; i<itemItems.size(); i++) {_topology.push_back(stoi(itemItems[i]));}
        }
        else {return 1;}
    }
    if (_alpha==0||_eta==0||_RASmooth==0|_topology.size()==0) {return 1;}
    else {return 0;}
}

// Feeds inputs forward through the network
void network::feed(const vector<double> &inputs) {
    assert(inputs.size()==_layers[0].size()-1);
    for (unsigned i=0; i<inputs.size(); i++) {
        _layers[0][i].set_out(inputs[i]);
    }
    for (unsigned l=1; l<_layers.size(); l++) {
        layer &llayer = _layers[l-1];
        for (unsigned n=0; n<_layers[l].size(); n++) {
            _layers[l][n].feed(llayer);
        }
    }
}

// Calculates & back-propagates network gradients
void network::learn(const vector<double> &outputs) {
    assert(outputs.size()==_layers.back().size()-1);
    layer &outlayer = _layers.back();

    // Compute current RMS error & modify recent average error
    _RMSE = 0;
    for (unsigned n=0; n<outlayer.size()-1; n++) {
        double delta = outputs[n]-outlayer[n].get_out();
        _RMSE += pow(delta,2);
    }
    _RMSE /= outlayer.size()-1;
    _RMSE = sqrt(_RMSE);
    _RAE = (_RAE*_RASmooth+_RMSE)/(_RASmooth+1);

    // Calculate gradients on network layers
    for (unsigned n=0; n<outlayer.size(); n++) {
        outlayer[n].outGrad(outputs[n]);
    }
    for (unsigned l=_layers.size()-2; l>0; l--) {
        layer &hlayer = _layers[l];
        layer &nlayer = _layers[l+1];
        for (unsigned n=0; n<hlayer.size(); n++) {
            hlayer[n].hidGrad(nlayer);
        }
    }

    // Update edge weights based on gradients
    for (unsigned l=_layers.size()-1; l>0; l--) {
        layer &tlayer = _layers[l];
        layer &llayer = _layers[l-1];
        for (unsigned n=0; n<tlayer.size()-1; n++) {
            tlayer[n].update(llayer,_alpha,_eta);
        }
    }
}

// Fills an input vector with the results of training
void network::report(vector<double> &results) const {
    results.clear();
    for (unsigned n=0; n<_layers.back().size()-1; n++) {
        results.push_back(_layers.back()[n].get_out());
    }
}

// Main program loop
int main(int argc, char* argv[]) {

    // Initialize the network from the config file
    network net("config");

    // Declare holding vectors for inputs, outputs, & results
    vector<double> inputs;
    vector<double> outputs;
    vector<double> results;

    // Loop over training epochs & write to results file
    ofstream outfile("network-results.txt");
    for (unsigned p=0; p<2000; p++) {

        // Produce random bits for inputs & compute XOR
        for (unsigned i=0; i<2; i++) {
            double in = rand()/double(RAND_MAX)>0.5 ? 1 : 0;
            inputs.push_back(in);
        }
        outputs.push_back(inputs[0]!=inputs[1]);

        // Propagate inputs & outputs through network
        net.feed(inputs);
        net.learn(outputs);
        net.report(results);

        // Report the outcome of this training epoch
        outfile << "\nEPOCH " << p+1 << "\n";
        outfile << "Inputs: ";
        for (unsigned i=0; i<2; i++) {outfile << inputs[i] << " ";}
        outfile << "\nTarget: " << outputs.back() << "\n";
        outfile << "Output: " << results.back() << "\n";
        outfile << "Recent Average Error: " << net.get_RAE() << "\n";

        // Clear input, output, & results vectors for the next epoch
        inputs.clear();
        outputs.clear();
        results.clear();
    }
    
    return 0;
}