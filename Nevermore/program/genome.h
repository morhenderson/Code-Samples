/*
    genome.h
    Morgan Henderson, Spring 2022
    A class for handling information about an individual genome
*/

// A neuron within a neural network
class genome {

    public:

        // Constructs an instance of the genome class
        genome();
        genome(const string &parentGenes);

        // A string of genes in the genome
        string genes;  

        // The fitness of this genome
        double fitness;   
};

// Constructs a random genome
genome::genome() {

    // Construct a random genome & compute its fitness
    genes = "";
    fitness = 0;
    for (unsigned i=0; i<phrase.size(); i++) {
        genes += alph[rCharID(rng)];
        if (genes[i]==phrase[i]) {fitness+=1;}
    }
    fitness /= phrase.size();
}

// Constructs a genome from a parent's genes
genome::genome(const string &parentGenes) {

    // Begin with the parent genome & zero fitness
    genes = parentGenes;
    fitness = 0;
    for (unsigned i=0; i<phrase.size(); i++) {

        // Check for & apply mutation & fitness metric
        if (stdU(rng)<mutRate) {genes[i]=alph[rCharID(rng)];}
        if (genes[i]==phrase[i]) {fitness+=1;}
    }
    fitness /= phrase.size();
}