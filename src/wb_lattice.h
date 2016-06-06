#pragma once
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include "wb_framework.h"
#include "thulac_base.h"
#include "dat.h"
namespace thulac{
namespace hypergraph{


typedef Hypergraph<int,LatticeEdge> Graph;

/* 从分词词性标注的输出lattice，到超图的graph的转换
 * 边变成节点，节点变成若干边
 * */
void lattice_to_graph(Lattice& lattice,Graph& graph){
    graph.clear();
	//std::cerr<<"lattice_to_graph:"<<lattice.size()<<std::endl;
    for(int i=0;i<lattice.size();i++){
        graph.nodes.push_back(Graph::Node());
        graph.nodes.back().data=lattice[i];
        graph.nodes.back().gold_standard=0;
        graph.nodes.back().is_begin=(lattice[i].begin==0)?1:0;
        graph.nodes.back().is_end=0;
    }
//    std::cout<<graph.nodes.size()<<"\n";
    std::vector< std::vector<int> > ends;
    for(int i=0;i<graph.nodes.size();i++){
        const LatticeEdge& lattice_edge=graph.nodes[i].data;
        int end=lattice_edge.begin+lattice_edge.word.size();
//        std::cout<<lattice_edge.begin<<" "<<end<<" "<<"\n";
       // put_raw(lattice_edge.word);
        while(ends.size()<=end)ends.push_back(std::vector<int>());
        ends[end].push_back(i);
    }
    for(int i=0;i<graph.nodes.size();i++){
        LatticeEdge& lattice_edge=graph.nodes[i].data;
        int begin=lattice_edge.begin;
        int end=lattice_edge.begin + lattice_edge.word.size();
        if(end==ends.size()-1){//end of the sentence
            graph.nodes[i].is_end=1;
        }
        for(int j=0;j<ends[begin].size();j++){
            int ind=ends[begin][j];
            graph.edges.push_back(Graph::Edge());
            graph.edges.back().to=&graph.nodes[i];
            graph.edges.back().from.push_back(&(graph.nodes[ind]));
        }
    }
};

void graph_to_lattice(Graph&graph, Lattice& lattice,int only_result=0){
    lattice.clear();
    for(int i=0;i<graph.nodes.size();i++){
        if(!(only_result)||(graph.nodes[i].result==1)){
            lattice.push_back(graph.nodes[i].data);
        }
    }
};


class LearningGraph: public Graph{
};


class LatticeIO:public DataIO<int,LatticeEdge>{
    const char* filename;
    std::fstream* fstream;
    int use_std;
    char mode;
public:
    LatticeIO(const char* filename,const char mode){
        use_std=false;
        this->filename=filename;
        this->mode=mode;
        if(filename[0]=='-'){
            std::cout<<"use stdio\n";
            use_std=true;
        }
        if(mode=='r'){
            fstream=new std::fstream(filename,std::fstream::in);
        }else{
            fstream=new std::fstream(filename,std::fstream::out);
        }

    };
    void reset(){
        //if(fstream)fstream->close();
        //std::cout<<"reset.\n";
        delete fstream;
        if(mode=='r'){
            fstream=new std::fstream(filename,std::fstream::in);
        }else{
        }
    }
    /*
     * load lines, and make the graph
     * */ 
    int load(Graph& graph){
        if(use_std){
            Lattice lattice;
            if(mode=='r'){
                std::cin>>lattice;
                //std::cout<<"hehe";
                if((lattice.size())==0)return 0;
                lattice_to_graph(lattice,graph);
            }
            return 1;
        }
        //std::cout<<"what\n";
        if(mode=='w')return 0;
        std::string str;
        if(!std::getline(*fstream,str))return 0;
        //std::cout<<str<<"\n";
        readline(str,graph);
        //if(graph.nodes.size()==2)std::cout<<graph.nodes.size()<<"\n";
        //std::cout<<str<<"\n";
        return 1;
    };
    int save(Graph& graph){
        if(mode=='r')return 0;
        int start=1;
        
        std::ostream* os;
        if(use_std){
            os=&(std::cout);
        }else{
            os=fstream;
        }
        for(int i=0;i<graph.nodes.size();i++){
            if(graph.nodes[i].result==1){//如果是结果，才输出
            //if(graph.nodes[i].data.margin==0){
                if(start){
                    start=0;
                }else{
                    (*os)<<" ";
                }
                (*os)<<graph.nodes[i].data.word<<"_"<<graph.nodes[i].data.tag;
                //(*os)<<"/"<<graph.nodes[i].data.margin;
            }
        }
        (*os)<<"\n";
        os->flush();
    };
    ~LatticeIO(){
        fstream->flush();
        fstream->close();
        std::cout<<"closed\n";
        delete fstream;
    };
private:
    int split_item(std::string& item,int offset,std::string& str){
        int del_ind=item.find_first_of(',',offset);
        str=item.substr(offset,del_ind-offset);
        return del_ind+1;
    };
    inline int readline(std::string& line,Graph& graph){
        std::istringstream iss(line);
        std::string item;
        graph.nodes.clear();
        graph.edges.clear();
        
        //std::cout<<line<<"\n";


        while(iss){
            item.clear();
            iss>>item;
            if(!item.length())continue;
            int offset=0;
            std::string is_path_str;
            offset=split_item(item,offset,is_path_str);
            std::string begin_str;
            offset=split_item(item,offset,begin_str);
            std::string margin_str;
            offset=split_item(item,offset,margin_str);
            std::string tag_str;
            offset=split_item(item,offset,tag_str);
            
            graph.nodes.push_back(Graph::Node());

            LatticeEdge& lattice_edge=graph.nodes.back().data;
            lattice_edge.begin=atoi(begin_str.c_str());
            string_to_raw(item.substr(offset).c_str(),lattice_edge.word);
            lattice_edge.tag=tag_str;
            lattice_edge.margin=atoi(margin_str.c_str());
            
            graph.nodes.back().gold_standard=(is_path_str[0]=='1')?1:0;
            graph.nodes.back().is_begin=(begin_str[0]=='0')?1:0;
            graph.nodes.back().is_end=0;
        }
        
        std::sort(graph.nodes.begin(),graph.nodes.end(),&compare_nodes);
        std::vector< std::vector<int> > ends;
        for(int i=0;i<graph.nodes.size();i++){
            const LatticeEdge& lattice_edge=graph.nodes[i].data;
            int end=lattice_edge.begin+lattice_edge.word.size();
            while(ends.size()<=end)ends.push_back(std::vector<int>());
            ends[end].push_back(i);
        }
        for(int i=0;i<graph.nodes.size();i++){
            LatticeEdge& lattice_edge=graph.nodes[i].data;
            int begin=lattice_edge.begin;
            int end=lattice_edge.begin + lattice_edge.word.size();
            if(end==ends.size()-1){//end of the sentence
                graph.nodes[i].is_end=1;
            }
            for(int j=0;j<ends[begin].size();j++){
                int ind=ends[begin][j];
                graph.edges.push_back(Graph::Edge());
                graph.edges.back().to=&graph.nodes[i];
                graph.edges.back().from.push_back(&(graph.nodes[ind]));
            }
        }

        //std::cout<<"number of nodes: "<<graph.nodes.size()<<"\n";
        //std::cout<<"number of edges: "<<graph.edges.size()<<"\n";

    };
private:
    static bool compare_nodes(const Graph::Node& a,const Graph::Node& b){
        return a.data.begin<b.data.begin;
    };

};

class NodeFeature{
public:
    NodeFeature(){};
    virtual int generate(Graph::Node& node,Raw& key){};
    virtual void add_features(Graph::Node& node,std::vector<Raw>& keys){
        keys.push_back(Raw());
        if(generate(node,keys.back())!=0){
            keys.pop_back();
        };
    };
	virtual ~NodeFeature(){};
};


class EdgeFeature{
public:
    EdgeFeature(){};
    virtual int generate(Graph::Node& left,Graph::Node& right,Raw& key){};
    virtual void add_features(Graph::Node& left,Graph::Node& right,std::vector<Raw>& keys){
        keys.push_back(Raw());
        if(generate(left,right,keys.back())!=0){
            keys.pop_back();
        };
    };
	virtual ~EdgeFeature(){};
};


class GeneralNodeFeature : public NodeFeature{
public:
    int use_tag;
    int use_word;
    int use_binary_length;
    int use_margin;
    GeneralNodeFeature(){
        use_tag=0;
        use_word=1;
        use_binary_length=0;
        use_margin=0;
    };
    int generate(Graph::Node& node,Raw& key){
        if(use_word){
            key.push_back(' ');
            //key+=node.data.word;
			//modify by cxx 20130419 16:17
			Word tmpWord = node.data.word;
			for(int i = 0; i < tmpWord.size(); i ++){
				if(tmpWord[i] > 32 && tmpWord[i] < 128){
					tmpWord[i] += 65248;
				}
			}
			key += tmpWord;
        }
        if(use_tag){
            key.push_back(' ');
            key+=node.data.tag;
        }
        if(use_binary_length){
            key.push_back(' ');
            key.push_back((node.data.word.size()>1)?'m':'s');
        }
        if(use_margin){
            key.push_back(' ');
            if(node.data.margin==-1)return 1;
            if(node.data.margin==0){
                key.push_back('0');
            }else if(node.data.margin<=1000){
                key.push_back('1');
            }else if(node.data.margin<=2000){
                key.push_back('2');
            }else if(node.data.margin<=4000){
                key.push_back('3');
            }else if(node.data.margin<=8000){
                key.push_back('4');
            }else if(node.data.margin<=16000){
                key.push_back('5');
            }else{
                key.push_back('x');
            }
            //std::cout<<node.data.margin<<" "<<(char)key.back()<<"\n";
        }
        //std::cout<<node.data.word<<"_"<<node.data.tag<<" "<<key<<"\n";
        return 0;
    };
};

class GeneralEdgeFeature: public EdgeFeature{
public:
    int use_left_tag;
    int use_right_tag;
    int use_left_binary_length;
    int use_right_binary_length;
    GeneralEdgeFeature(){
        use_left_tag=1;
        use_right_tag=1;
        use_left_binary_length=0;
        use_right_binary_length=0;
    };
    int generate(Graph::Node& left,Graph::Node& right,Raw& key){
        if(use_left_tag){
            key.push_back(' ');
            key+=left.data.tag;
        }
        if(use_right_tag){
            key.push_back(' ');
            key+=right.data.tag;
        }
        if(use_left_binary_length){
            key.push_back(' ');
            key.push_back((left.data.word.size()>1)?'m':'s');
        }
        if(use_right_binary_length){
            key.push_back(' ');
            key.push_back((right.data.word.size()>1)?'m':'s');
        }

        return 0;
    };
};

class LatticeFeature: public Feature<int,LatticeEdge>{
public:
    /*
     * 这个回调函数可以是4种：
     * 1. 将arg0对应的特征值加到arg1
     * 2. 将arg0对应特征值加1
     * 3. 将arg0对应特征值减1
     * 4. 收集arg0，用以生成特征DAT
     * */
    typedef void(LatticeFeature::*p_feature_call_back)(Raw&,int&);
    DAT* dat; 
    std::string filename;
    std::vector<NodeFeature*> node_features;
    std::vector<EdgeFeature*> edge_features;
public:
    LatticeFeature(){
        this->dat=NULL;
        node_features.push_back(new GeneralNodeFeature());//<word>
        GeneralNodeFeature* p_gnf=new GeneralNodeFeature();//<word, tag>
        p_gnf->use_tag=1;
        node_features.push_back(p_gnf);
        p_gnf=new GeneralNodeFeature();//<tag>
        p_gnf->use_tag=1;p_gnf->use_word=0;
        node_features.push_back(p_gnf);
        p_gnf=new GeneralNodeFeature();//<len==1>
        p_gnf->use_binary_length=1;p_gnf->use_word=0;
        node_features.push_back(p_gnf);
        p_gnf=new GeneralNodeFeature();//<m>
        p_gnf->use_margin=1;p_gnf->use_word=0;
        node_features.push_back(p_gnf);
        p_gnf=new GeneralNodeFeature();//<tag,len==1>
        p_gnf->use_word=0;p_gnf->use_tag=1;p_gnf->use_binary_length=1;
        node_features.push_back(p_gnf);
        p_gnf=new GeneralNodeFeature();//<m,len==1>
        p_gnf->use_word=0;p_gnf->use_margin=1;p_gnf->use_binary_length=1;
        node_features.push_back(p_gnf);

        GeneralEdgeFeature* p_gef=new GeneralEdgeFeature();
        edge_features.push_back(p_gef);
        p_gef=new GeneralEdgeFeature();
        p_gef->use_left_binary_length=1;p_gef->use_right_binary_length=1;
        edge_features.push_back(p_gef);
    };
    ~LatticeFeature(){
        while(node_features.size()>0){
			//std::cerr<<"node size:"<<node_features.size()<<std::endl;
            delete node_features.back();
            node_features.pop_back();
        }
        while(edge_features.size()>0){
			//std::cerr<<"edge size:"<<edge_features.size()<<std::endl;
            delete edge_features.back();
            edge_features.pop_back();
        }
        delete dat;
    };
    int load(){
        this->dat=new DAT(filename.c_str());
    }
    inline void extract_features(
            Graph& graph
            ,p_feature_call_back call_back
            ,int filter=0
            ){
        RawSentence key;
        std::vector<Raw> keys;
        for(int i=0;i<graph.nodes.size();i++){
            Graph::Node& node=graph.nodes[i];
            LatticeEdge& lattice_edge=node.data;
            node.weight=0;
            if(filter!=0){
                if((filter==1)&&(node.gold_standard!=1))
                    continue;
                if((filter==-1)&&(node.result!=1))
                    continue;
            };

            for(int fid=0;fid<node_features.size();fid++){
                //continue;
                keys.clear();
                node_features[fid]->add_features(node,keys);
                for(int j=0;j<keys.size();j++){
                    key.clear();
                    key+='u';
                    key.push_back(fid+33);
                    key+=keys[j];
                    //put_raw(key);putchar('\n');
                    (this->*call_back)(key,node.weight);
                }
                //std::cout<<fid<<"\n";
                /*key.clear();
                key+='u';
                key.push_back(fid+33);
                if(node_features[fid]->generate(node,key)==0)
                    (this->*call_back)(key,node.weight);
                    */
            }
        }
        for(int i=0;i<graph.edges.size();i++){
            Graph::Edge& edge=graph.edges[i];
            Graph::Node& right_node=*edge.to;
            Graph::Node& left_node=*edge.from[0];
            edge.weight=0;
            if(filter!=0){
                int flag=1;
                if((filter==1)&&(edge.to->gold_standard!=1))
                    continue;
                if((filter==-1)&&(edge.to->result!=1))
                    continue;
                for(int j=0;j<edge.from.size();j++){
                    if((filter==1)&&(edge.from[j]->gold_standard!=1)){
                        flag=0;break;
                    }
                    if((filter==-1)&&(edge.from[j]->result!=1)){
                        flag=0;break;
                    }
                }
                if(flag==0)continue;
            }
            for(int fid=0;fid<edge_features.size();fid++){
                //continue;
                keys.clear();
                edge_features[fid]->add_features(left_node,right_node,keys);
                for(int j=0;j<keys.size();j++){
                    key.clear();
                    key+='b';
                    key.push_back(fid+33);
                    key+=keys[j];
                    (this->*call_back)(key,edge.weight);
                }
                /*
                key.clear();
                key+='b';
                key.push_back(fid+33);
                if(edge_features[fid]->generate(left_node,right_node,key)==0)
                    (this->*call_back)(key,edge.weight);*/
                    
            }
        }
    };
    void add_weights_call_back(Raw& key,int& weight){
        int ind=dat->match(key);
        if(ind==-1)return;
        //printf("%d\n",dat->dat[ind].base);
        weight+=dat->dat[ind].base;
    };
    void add_weights(Graph& graph){
        p_feature_call_back pp;
        pp=(p_feature_call_back)(&LatticeFeature::add_weights_call_back);
        extract_features(graph,pp);
    };

};

class LatticeFeatureLearner: public LatticeFeature, public FeatureLearner<int,LatticeEdge>{
public:
    int threshold;
    Counter<Raw> feature_counter;
    double* average;
private:
    int delta;
    int step;
public:
    void add_weights(Graph& graph){
        p_feature_call_back pp;
        pp=(p_feature_call_back)(&LatticeFeature::add_weights_call_back);
        extract_features(graph,pp);
    };

    LatticeFeatureLearner(){
        threshold=0;
    };
    ~LatticeFeatureLearner(){
        delete average;
    };
    void feature_selection_call_back(Raw& key,int& weight){
        feature_counter.update(key);
    };

    void extract(Graph& graph){
        p_feature_call_back pp;
        pp=(p_feature_call_back)(&LatticeFeatureLearner::feature_selection_call_back);
        extract_features(graph,pp);
    };
    void make_features(){
        std::vector<DATMaker::KeyValue> kv;
        for(Counter<Raw>::iterator it=feature_counter.begin();it!= feature_counter.end();++it){
            //put_raw(it->first);
            if(it->second<threshold){
                //feature_counter.erase(it);
                continue;
            }
            //std::cout<<" "<<it->second<<"\n";
            kv.push_back(DATMaker::KeyValue());
            kv.back().key=it->first;
            kv.back().value=0;
        }
        std::cout<<"features: "<<kv.size()<<"\n";
        DATMaker* dat=new DATMaker();
        dat->make_dat(kv);
        dat->shrink();
        //std::cout<<dat->dat_size<<"\n";
        /*Raw raw;
        string_to_raw(std::string("w 我们"),raw);
        std::cout<<dat->match(raw)<<"\n";

        string_to_raw(std::string("w 我"),raw);
        std::cout<<dat->dat[dat->match(raw)].base<<"\n";*/
        this->dat=dat;
        average=new double[dat->dat_size];
        for(int i=0;i<dat->dat_size;i++)average[i]=0;
    };
    void update_call_back(Raw& key,int&weight){
        int ind=dat->match(key);
        if(ind==-1)return;
        dat->dat[ind].base+=this->delta;
        average[ind]+=step*this->delta;
    };
    void update(Graph& graph,int delta,long step){
        for(int i=0;i<graph.nodes.size();i++){
            if(graph.nodes[i].result==1){
                //std::cout<<graph.nodes[i].data.word<<"_"<<graph.nodes[i].data.tag<<" ";
            }
        };
        //std::cout<<"\n";
        this->step=step;
        this->delta=delta;
        p_feature_call_back pp;
        pp=(p_feature_call_back)(&LatticeFeatureLearner::update_call_back);
        extract_features(graph,pp,1);
        this->delta=-delta;
        extract_features(graph,pp,-1);
    };
    int load(){};
    void save(long step){//average
        for(int i=0;i<dat->dat_size;i++){
            int ind=dat->dat[i].check;
            if((ind<0)||(ind>=dat->dat_size))continue;
            if(dat->dat[ind].base!=i)continue;
            dat->dat[i].base=(int)(
                    (
                        (double)dat->dat[i].base
                        -
                        average[i]/step
                    )
                    *1000+0.5
                    );
        };
        /*Raw raw;
        string_to_raw(std::string("u! 0"),raw);
        std::cout<<dat->dat[dat->match(raw)].base<<"\n";

        string_to_raw(std::string("u! x"),raw);
        std::cout<<dat->dat[dat->match(raw)].base<<"\n";*/
        this->dat->save(this->filename.c_str());
    };
};


}//end of hypergraph
}//end of thulac
