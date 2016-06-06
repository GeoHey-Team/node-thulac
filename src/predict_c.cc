#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <getopt.h>
#include "preprocess.h"
#include "cb_tagging_decoder.h"

using namespace thulac;

int main (int argc,char **argv) {
    /*
     int show_sentence=false;
     
     FILE* input_file=stdin;
     FILE* output_file=stdin;
     int threshold=0;
     static struct option long_options[] =
     {
     //{"verbose", no_argument,       &verbose_flag, 1},
     //{"seg_only",   no_argument,       &seg_only, 1},
     //{"help",     no_argument,       0, 'h'},
     {"threshold",     required_argument,       0, 't'},
     {"show_input",  no_argument,       0, 'T'},
     {"input",  required_argument,       0, 'i'},
     {"output",  required_argument,       0, 'o'},
     
     {0, 0, 0, 0}
     };
     int c;
     int option_index = 0;
     while ( (c = getopt_long(argc, argv, "o:i:Tt:h",long_options,&option_index)) != -1) {
     switch (c) {
     case 0:
     break;
     case 'T':
     show_sentence=true;
     break;
     case 'i':
     input_file=fopen(optarg,"r");
     break;
     case 'o':
     output_file=fopen(optarg,"r");
     break;
     case 't' :
     threshold=atoi(optarg);
     break;
     case 'h' :
     case '?' :
     default :
     fprintf(stderr,"");
     return 1;
     }
     }
     
     
     
     
     if(!(optind<argc)){
     fprintf(stderr,"need one augument for prefix for model files\n");
     return 1;
     }
     */
    int show_sentence=true;
    int threshold=0;
    std::string prefix = "model_c";
    
    TaggingDecoder* tagging_decoder=new TaggingDecoder();
    tagging_decoder->separator = '_';
    tagging_decoder->threshold=threshold*1000;
    permm::Model* tagging_model = new permm::Model((prefix+"_model.bin").c_str());
    DAT* tagging_dat = new DAT((prefix+"_dat.bin").c_str());
    char** tagging_label_info = new char*[tagging_model->l_size];
    int** tagging_pocs_to_tags = new int*[16];
    
    get_label_info((prefix+"_label.txt").c_str(), tagging_label_info, tagging_pocs_to_tags);
    tagging_decoder->init(tagging_model, tagging_dat, tagging_label_info, tagging_pocs_to_tags);
    tagging_decoder->set_label_trans();
    
    Preprocesser* preprocesser = new Preprocesser();
    
    POCGraph poc_cands;
    POCGraph new_poc_cands;
    int rtn=1;
    thulac::RawSentence oriRaw;
    thulac::RawSentence raw;
    thulac::SegmentedSentence segged;
    thulac::TaggedSentence tagged;
    const int BYTES_LEN=10000;
    char* s=new char[ BYTES_LEN ];
    char* out=new char[BYTES_LEN];
    
    std::ostringstream ost;
    std::string ori = "就咋了 爱咋咋地";
    
    //while(std::getline(std::cin,ori)){
    //while(std::getline(std::cin,ori)){
    //	if(ori.length()>9999){
    //		ori = ori.substr(0,9999);
    //	}
    
    strcpy(s,ori.c_str());
    size_t in_left=ori.length();
    
    thulac::get_raw(oriRaw,s,in_left);
    
    preprocesser->clean(oriRaw,raw,poc_cands);
    
    if(raw.size()){
        //tagging_decoder->segment(raw, poc_cands, tagged);
        tagging_decoder->segment(raw);
        if(tagging_decoder->threshold==0){
            tagging_decoder->output_sentence(out);
        }else{
            if(show_sentence){
                tagging_decoder->output_raw_sentence();
                printf(" ");
            }
            tagging_decoder->cal_betas();
            tagging_decoder->output_allow_tagging();
        }
        //std::cout<<tagged;
    }
    //putchar(10);
    //std::cout.flush();
    //}
    
    std::cout << out << std::endl;
    
    delete tagging_decoder;
    if(tagging_model != NULL){
        for(int i = 0; i < tagging_model->l_size; i ++){
            if(tagging_label_info) delete[](tagging_label_info[i]);
        }
    }
    delete[] tagging_label_info;
    
    if(tagging_pocs_to_tags){
        for(int i = 1; i < 16; i ++){
            delete[] tagging_pocs_to_tags[i];
        }
    }
    delete[] tagging_pocs_to_tags;
    
    delete tagging_dat;
    if(tagging_model!=NULL) delete tagging_model;
    
    //    if(input_file!=stdin)fclose(input_file);
    //    if(output_file!=stdin)fclose(output_file);
    
    return 0;
}


