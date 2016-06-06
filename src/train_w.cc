#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include "wb_lattice.h"
#include "wb_extended_features.h"


using namespace thulac;
using namespace hypergraph;

    
int main (int argc,char **argv) {

    int iteration=15;
    static struct option long_options[] =
		{
			{"help",     no_argument,       0, 'h'},
			{"iteration",  required_argument,       0, 'i'},
			{0, 0, 0, 0}
		};
    int c;
    int option_index = 0;
    DAT* sogouw=NULL;
    DAT* sogout=NULL;
    std::vector<std::string> n_gram_model;
    std::vector<std::string> dictionaries;
    while ( (c = getopt_long(argc, argv, "T:W:i:hg:",long_options,&option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'g':
                n_gram_model.push_back(std::string(optarg));
                break;
            case 'W':
                dictionaries.push_back(std::string(optarg));
                sogouw=new DAT(optarg);
                break;
            case 'T':
                sogout=new DAT(optarg);
                break;
            case 'i' : 
                iteration=atoi(optarg);
                break;
            case 'h' :
            case '?' : 
            default : 

                fprintf(stderr,"");
                return 1;
        }
    }
    if(!(optind+1<argc)){
        fprintf(stderr,"need two auguments for training file and prefix for model files\n");
        return 1;
    }

    DataIO<int,LatticeEdge>* io=NULL;
    
    //learning
    io=new LatticeIO(argv[optind],'r');
    LatticeFeatureLearner lfl;
    lfl.filename=argv[optind+1];
    /*if(sogouw){
        lfl.node_features.push_back(new DictNodeFeature(sogouw));
    }*/
    if(sogout){
        lfl.node_features.push_back(new SogouTFeature(sogout));
    }
    for(int i=0;i<dictionaries.size();++i){
        lfl.node_features.push_back(new DictNodeFeature(new DAT(dictionaries[i].c_str())));
    }
    if(n_gram_model.size()){
        for(int i=0;i<n_gram_model.size();i++){
            lfl.node_features.push_back(new NGramFeature(
                        new language_model::BigramModel(n_gram_model[i].c_str())));
        }
    }

    Learner<int,LatticeEdge> learner;
    //printf("what\n");
    learner.learn(*io,iteration,lfl);

    delete io;
    delete sogouw;
    delete sogout;
}

