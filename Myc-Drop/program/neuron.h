/*
    neuron.h
    Morgan Henderson, Spring 2022
    A class for handling information about a neuron
*/

// An edge between neurons
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

        // Constructs an instance of the neuron class
        neuron(const unsigned &nout, const unsigned &index);

        // Get & set functions for the neuron's output
        void set_out(const double &output) {_out = output;};
        double get_out(void) const {return _out;}

        // Computes gradients for output & hidden layers
        void outGrad(const double &output);
        void hidGrad(const layer &nlayer);

        // Feeds input into the neuron from the previous layer
        void feed(const layer &llayer);

        // Updates the neuron's output value
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

// Computes gradients for output layer neurons
void neuron::outGrad(const double &output) {
    double delta = output-_out;
    _grad = delta*_dactivate(_out);
}

// Computes gradients for hidden layer neurons
void neuron::hidGrad(const layer &nlayer) {
    double DOW = _sumDOWs(nlayer);
    _grad = DOW*_dactivate(_out);
}

// The neuron's activation function
double neuron::_activate(const double &input) {
    return tanh(input);
}

// The derivative of the neuron's activation function
double neuron::_dactivate(const double &input) {
    return 1-pow(tanh(input),2);
}

// Sums the derivative of weights in the next layer
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