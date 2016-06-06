#pragma once
#include <math.h>
#include "thulac_base.h"
#include "dat.h"
namespace thulac{

namespace language_model{

/*
 * the node with KN smooth
 * */
class KNNode{

};

class BigramModel{
private:
    //const static Character del='$';
    Character del;
private:
    int unigram_node_index;
    DAT* dat;
    double d1;
    double d0;
    int n_token;
    int n_type;
    
public:
    BigramModel(const char* filename){
        del='$';
        d1=0.3;
        d0=0.3;
        dat=new DAT(filename);
        unigram_node_index=dat->get_index(dat->get_index(0,' '),'u');
        int index=dat->get_index(unigram_node_index,' ');
        n_token=dat->dat[
            dat->get_index(dat->get_index(index,'t'),0)
            ].base;
        n_type=dat->dat[
            dat->get_index(dat->get_index(index,'T'),0)
            ].base;

    }
    ~BigramModel(){
        delete dat;
    }
    inline double get_p(const Character& b){
        //std::cout<<"zhang "<<(int)b<<"\n";
        int index=dat->get_index(unigram_node_index,b);
        //std::cout<<n_token<<" "<<n_type<<"\n";
        double p=0;
        if(index!=-1){
            p=((double)(dat->dat[dat->get_index(index,0)].base-d0))
                    /
                    n_token;
        }
        //std::cout<<"half p "<<p<<"\n";
        p+=n_type*d0/n_token*get_p();
        //std::cout<<"unigram "<<p<<"\n";
        return p;
    };
    inline double get_p(){
        //std::cout<<"me "<<1.0/(n_type+1)<<"\n";
        return 1.0/(n_type+1);
    };
    //basic bigram node
    inline double get_p(const Character& a,const Character& b){
        //std::cout<<"a b "<<(int)a<<" "<<(int)b<<"\n";
        double p=0;
        int index=0;
        index=dat->get_index(0,a);
        if(index==-1){//no a, goto unigram
            return get_p(b);
            //Unigram
        }else{
            int bigram_index=dat->get_index(index,b);
            index=dat->get_index(index,' ');
            int n_token=dat->dat[
                dat->get_index(dat->get_index(index,'t'),0)
                ].base;
            int n_type=dat->dat[
                dat->get_index(dat->get_index(index,'T'),0)
                ].base;
            //std::cout<<n_token<<" "<<n_type<<"\n";
            if(bigram_index!=-1){//has b
                p=((double)(dat->dat[dat->get_index(bigram_index,0)].base-d1))
                    /
                    n_token;
            }
            p+=n_type*d1/n_token*get_p(b);
        }

        return p;
    }
    
    double get_log_p(const Raw& raw){
        //std::cout<<raw<<"\n";
        double log_p=0;
        if(raw.size()==0)return log_p;
        log_p+=log(get_p(del,raw[0]));
        //std::cout<<"log_p "<<log_p<<"\n";
        for(int i=0;i<raw.size()-1;i++){
            log_p+=log(get_p(raw[i],raw[i+1]));
            //std::cout<<"log_p "<<log_p<<"\n";
        }
        log_p+=log(get_p(raw[raw.size()-1],del));
        //std::cout<<"log_p "<<log_p<<"\n";
        return log_p;
    }
};

}//end of langauge model

}//end of thulac
