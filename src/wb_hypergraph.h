#pragma once
#include <vector>
#include <algorithm>

namespace thulac{
namespace hypergraph{

template<typename ValueType,typename NodeData>
class Hypergraph{
public:
    struct Node;
    struct Edge;

    struct AlphaBeta{
        ValueType value;
        Edge* edge;
    };


    struct Node{
        char is_begin;
        char is_end;
        int result;//used to mark an optimal path
        int gold_standard;
        ValueType weight;
        AlphaBeta alpha;
        AlphaBeta beta;
        NodeData data;
    };

    struct Edge{
        Node* to;
        std::vector<Node*> from;
        ValueType weight;
    };

public:
    std::vector<Node> nodes;
    std::vector<Edge> edges;

public:
    void clear(){
        nodes.clear();
        edges.clear();
    }
    /*perfrom the algorithm and return the best value*/
    inline ValueType forward_algorithm(){
        ValueType value;
        int flag;
        ValueType best_value;
        Node* best_node=NULL;
        //initial
        for(size_t i=0;i<nodes.size();i++){
            nodes[i].alpha.edge=NULL;
            nodes[i].result=0;
            if(nodes[i].is_begin==1){
                nodes[i].alpha.value=nodes[i].weight;
            }
            if(nodes[i].is_end==1&&nodes[i].is_begin==1){
                if(best_node==NULL || best_value<nodes[i].weight){
                    best_value=nodes[i].weight;
                    best_node=&nodes[i];
                }
            }
        }
        //DP search
        for(size_t i=0;i<edges.size();i++){
            Edge& edge=edges[i];
            flag=true;
            value=0;
            for(size_t j=0;j<edge.from.size();j++){
                if(edge.from[j]->alpha.edge==NULL && !edge.from[j]->is_begin){
                    flag=false;break;
                }else{
                    value+=edge.from[j]->alpha.value;
                }
            }
            if(!flag)
                continue;
        
            value+=edge.weight;
            if(!edge.to->alpha.edge || edge.to->alpha.value<value+edge.to->weight){
                edge.to->alpha.edge=&edge;
                edge.to->alpha.value=value+edge.to->weight;
                if(edge.to->is_end){
                    if(best_node==NULL || best_value<edge.to->alpha.value){
                        best_node=edge.to;
                        best_value=edge.to->alpha.value;
                    }
                }
            }
        }
        //backtracking
		best_node->result=1;
		//std::cerr<<"backstracking:"<<edges.size()<<std::endl;
        for(int i=edges.size()-1;i>=0;i--){
            Edge& edge=edges[i];
            if(edge.to->result&&edge.to->alpha.edge==&edge){
                for(int j=0;j<edge.from.size();j++)edge.from[j]->result=1;
            }
        }
		//std::cerr<<"backstracking done"<<std::endl;
        return best_value;
    };
    inline void backward_algorithm(){

    };
    inline void eval(int& std,int&rst,int&cor){
        for(int i=0;i<nodes.size();i++){
            if(nodes[i].result==1)rst++;
            if(nodes[i].gold_standard==1)std++;
            if((nodes[i].result==1)&&(nodes[i].gold_standard))cor++;
        }
    };
private:
    
};

}//end of hypergraph
}//end of thulac


