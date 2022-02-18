/*
    network.h
    Morgan Henderson, Spring 2022
    A class for handling information about a network
*/

// A neural network
class network {

    public:

        // Constructs an instance of the network class
        network();

        // Feeds inputs into the network
        void feed(const vector<double> &inputs);

        // Performs back-propagation for the network
        void learn(const vector<double> &outputs);

        // Reports the network's outputs & recent average error
        void report(vector<double> &results) const;
        double get_RAE(void) {return _RAE;}

    private:

        // The network's topology & its layers of neurons
        vector<layer> _layers;

        // The network's root mean square & recent average error
        double _RMSE;
        double _RAE;
};

// Initializes an instance of the network class
network::network() {

    // Add input/output layers to the network's topology
    topology.insert(topology.begin(),inIDs.size());
    topology.push_back(1);

    // Construct the network layer by layer
    for (unsigned l=0; l<topology.size(); l++) {
        _layers.push_back(layer());
        unsigned nout = l==topology.size()-1 ? 0 : topology[l+1];
        for (unsigned n=0; n<=topology[l]; n++) {
            _layers.back().push_back(neuron(nout,n));
        }
        _layers.back().back().set_out(1.0);
    }

    // Initialize the network root mean square & recent average errors
    _RMSE = 0;
    _RAE = 0;
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
    _RAE = (_RAE*RASmooth+_RMSE)/(RASmooth+1);

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
            tlayer[n].update(llayer,alpha,eta);
        }
    }
}

// Fills a vector with the outputs of output layer neurons
void network::report(vector<double> &results) const {
    results.clear();
    for (unsigned n=0; n<_layers.back().size()-1; n++) {
        results.push_back(_layers.back()[n].get_out());
    }
}