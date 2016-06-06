#pragma once
#include "wb_lattice.h"
#include "bigram_model.h"

namespace thulac{
namespace hypergraph{
/**
 * 节点的扩展特征
 * 1 词典信息，带词性
 * 2 SogouT的RAV
 * 3 SogouT的新信息
 * 4 串的命名体生成概率
 * */
class SogouTFeature : public NodeFeature{
public:
    DAT* dat;
public:
    SogouTFeature(DAT* dat){
        this->dat=dat;
    };
    ~SogouTFeature(){
		delete dat;
	};
    void add_features(Graph::Node& node,std::vector<Raw>& keys){
        int base=dat->match(node.data.word);
        if(base>0){
            int word_base=dat->dat[dat->dat[base].check].base;
            int count=dat->dat[base].base;
            //std::cout<<node.data.word<<" ";
            //std::cout<<count<<"\n";
            //if(count>10)return;

            keys.push_back(Raw());
            Raw& key=keys.back();
            key+=node.data.tag;
            key.push_back(' ');
            key.push_back(count+'a');
            //std::cout<<count<<"\n";
            return;
            int rav=count/3+'a';
            word_base=dat->dat[word_base+'_'].base;
            count=dat->dat[word_base].base;
            //std::cout<<count<<"\n";
            //return;
            //if(count>10)count=10;
            for(int i=0;i<count;i++){
                int tag_ind=dat->dat[dat->dat[word_base+i+19968].base].base;
                //std::cout<<tag_ind<<"\n";
                keys.push_back(Raw());
                Raw& raw=keys.back();
                raw+=node.data.tag;
                raw.push_back(' ');
                raw.push_back(tag_ind);
                raw.push_back(rav);
            }
        }
        //keys.push_back();
        
    };
};
class DictNodeFeature : public NodeFeature{
    /*类似于SogouW的词典信息*/
public:
    DAT* dat;
public:
    DictNodeFeature(DAT* dat){
        this->dat=dat;
    };
    ~DictNodeFeature(){
		delete dat;
    };

    void add_features(Graph::Node& node,std::vector<Raw>& keys){
        int base=dat->match(node.data.word);
        if(base>0){
            int word_base=dat->dat[dat->dat[base].check].base;
            int count=dat->dat[base].base;
            if(count>10)return;
            for(int i=0;i<count;i++){
                int tag_ind=dat->dat[dat->dat[word_base+i+'1'].base].base;
                keys.push_back(Raw());
                Raw& raw=keys.back();
                raw+=node.data.tag;
                if(tag_ind<'a' || tag_ind>'o'){
                    std::cout<<node.data.word<<"\n";
                    std::cout<<i<<" "<<(char)tag_ind<<" "<<tag_ind<<" oh\n";
                }
                raw.push_back(' ');
                raw.push_back(tag_ind);
            }
        }
    };
};



class NGramFeature : public NodeFeature{
    /*ngram特征*/
public:
    language_model::BigramModel* bm;
public:
    NGramFeature(language_model::BigramModel* bm){
//        std::cout<<"loaded\n";
        this->bm=bm;
    };
    ~NGramFeature(){
        delete bm;
    };
    //将node中的信息生成keys
    void add_features(Graph::Node& node,std::vector<Raw>& keys){
        double p=bm->get_log_p(node.data.word);
        int ip=100+(int)(p/5);
        ip=ip>20?ip:20;
        //std::cout<<node.data.word<<" "<<ip<<"\n";

        keys.push_back(Raw());
        Raw& key=keys.back();
        key+=node.data.tag;
        key.push_back(' ');
        key.push_back('0'+node.data.word.size());
        key.push_back(ip);
    };
};

}//end of thulac
}//end of thulac
