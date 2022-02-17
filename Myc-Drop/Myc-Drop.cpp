/*/ 
    main.cpp
    Morgan Henderson, August 17, 2021
    A program for a simple, fully connected neural network model
/*/

// Define a structure for the edges within a network
struct edge {
    double weight;
    double delta;
};

// Declare the neuron class & create a typedef for layers of neurons
class neuron;
typedef vector<neuron> layer;

// A neuron within a neural network
class neuron {
    public:
        neuron(const unsigned &nout, const unsigned &index);
        void set_out(const double &output) {_out = output;};
        double get_out(void) const {return _out;}
        void feed(const layer &llayer);
        void outGrad(const double &output);
        void hidGrad(const layer &nlayer);
        void update(layer &llayer);
    private:
        static double eta;
        static double alpha;
        double _out;
        vector<edge> _edges;
        unsigned _index;
        double _grad;
        static double randweight() {return rand()/double(RAND_MAX);}
        static double activate(const double &input);
        static double dactivate(const double &input);
        double sumDOWs(const layer &nlayer) const;
};

// Initializes an instance of the neuron class
neuron::neuron(const unsigned &nout, const unsigned &index) {
    for (unsigned e=0; e<nout; e++) {
        _edges.push_back(edge());
        _edges.back().weight = randweight();
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
    _out = activate(sum);
}

// Computes gradients for the output layer
void neuron::outGrad(const double &output) {
    double delta = output-_out;
    _grad = delta*dactivate(_out);
}

// Computes gradients for hidden layers
void neuron::hidGrad(const layer &nlayer) {
    double DOW = sumDOWs(nlayer);
    _grad = DOW*dactivate(_out);
}

// The activation function
double neuron::activate(const double &input) {
    return tanh(input);
}

// The derivative of the activation function
double neuron::dactivate(const double &input) {
    return 1-pow(tanh(input),2);
}

// Sums the derivative of weights in the following layer
double neuron::sumDOWs(const layer &nlayer) const {
    double sum = 0.;
    for (unsigned n=0; n<nlayer.size()-1; n++) {
        sum += _edges[n].weight*nlayer[n]._grad;
    }
    return sum;
}

// Updates the previous layer weights associated with a neuron
void neuron::update(layer &llayer) {
    for (unsigned n=0; n<llayer.size(); n++) {
        neuron &neighbor = llayer[n];
        double deltaOld = neighbor._edges[_index].delta;
        double delta = eta*neighbor.get_out()*_grad + alpha*deltaOld;
        neighbor._edges[_index].delta = delta;
        neighbor._edges[_index].weight += delta;
    }
}

// A neural network
class network {
    public:
        network(const vector<unsigned> &topology);
        void feed(const vector<double> &inputs);
        void learn(const vector<double> &outputs);
        void report(vector<double> &results) const;
        double get_RAE(void) {return _RAE;}

    private:
        vector<layer> _layers;
        double _RMSE;
        double _RAE;
        static double _RASmooth;
};

// Initializes an instance of the network class
network::network(const vector<unsigned> &topology) {
    const unsigned nlayers = topology.size();
    for (unsigned l=0; l<nlayers; l++) {
        _layers.push_back(layer());
        unsigned nout = l==nlayers-1 ? 0 : topology[l+1];
        for (unsigned n=0; n<=topology[l]; n++) {
            _layers.back().push_back(neuron(nout,n));
        }
        _layers.back().back().set_out(1.0);
    }
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
            tlayer[n].update(llayer);
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

// Set static class members
double neuron::eta = 0.15;
double neuron::alpha = 0.5;
double network::_RASmooth = 3;