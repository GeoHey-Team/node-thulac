#ifndef __wrapper__hh__
#define __wrapper__hh__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <string>
#include <memory>

#define TOSTR(obj) (*v8::String::Utf8Value((obj)->ToString()))

#include "cb_tagging_decoder.h"
#include "chinese_charset.h"
#include "preprocess.h"
#include "postprocess.h"
#include "thulac_base.h"
#include "thulac.h"
#include "timeword.h"
#include "verbword.h"
#include "negword.h"
#include "punctuation.h"
#include "filter.h"
#include "wb_extended_features.h"
#include "wb_lattice.h"
#include "bigram_model.h"
using namespace thulac;

bool is_file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

class Segmentor: public Nan::ObjectWrap {
public:
    static Nan::Persistent<v8::FunctionTemplate> constructor;
    static void Initialize(v8::Local<v8::Object> target);
    static NAN_METHOD(New);

    static NAN_METHOD(load_model);
    static NAN_METHOD(predict);

    Segmentor();

private:
    ~Segmentor();

    TaggingDecoder* tagging_decoder_;
    TaggingDecoder* cws_decoder_;
    permm::Model* tagging_model_;
    permm::Model* cws_model_;

    DAT* tagging_dat_;
    DAT* cws_dat_;

    char** tagging_label_info_;
    int** tagging_pocs_to_tags_;
    char** cws_label_info_;
    int** cws_pocs_to_tags_;


    thulac::hypergraph::LatticeFeature* lf_;
    DAT* sogout_;
    hypergraph::Decoder<int, LatticeEdge> decoder_;

    Preprocesser* preprocesser_;

    Postprocesser* ns_dict_;
    Postprocesser* idiom_dict_;
    Postprocesser* nz_dict_;
    Postprocesser* ni_dict_;
    Postprocesser* noun_dict_;
    Postprocesser* adj_dict_;
    Postprocesser* verb_dict_;
    Postprocesser* vm_dict_;
    Postprocesser* y_dict_;

    Postprocesser* user_dict_;

    Punctuation* punctuation_;

    NegWord* negword_;
    TimeWord* timeword_;
    VerbWord* verbword_;
    Filter* filter_;

    Character separator_;
    bool useT2S_;
    bool seg_only_;
    bool useFilter_;
    bool use_second_;
    std::string userword_;
};


//  implementation

Nan::Persistent<v8::FunctionTemplate> Segmentor::constructor;

void Segmentor::Initialize(v8::Local<v8::Object> target) {
    Nan::HandleScope scope;

    v8::Local<v8::FunctionTemplate> lcons = Nan::New<v8::FunctionTemplate>(Segmentor::New);
    lcons->InstanceTemplate()->SetInternalFieldCount(1);
    lcons->SetClassName(Nan::New("Segmentor").ToLocalChecked());

    Nan::SetPrototypeMethod(lcons, "loadModel", load_model);
    Nan::SetPrototypeMethod(lcons, "predict", predict);

    target->Set(Nan::New("Segmentor").ToLocalChecked(),lcons->GetFunction());
    constructor.Reset(lcons);
}

Segmentor::Segmentor() : Nan::ObjectWrap() {
    lf_ = NULL;
    sogout_ = NULL;

    preprocesser_ = NULL;

    ns_dict_ = NULL;
    idiom_dict_ = NULL;
    nz_dict_ = NULL;
    ni_dict_ = NULL;
    noun_dict_ = NULL;
    adj_dict_ = NULL;
    verb_dict_ = NULL;
    vm_dict_ = NULL;
    y_dict_ = NULL;

    user_dict_ = NULL;

    punctuation_ = NULL;

    negword_ = NULL;
    timeword_ = NULL;
    verbword_ = NULL;
    filter_ = NULL;
}

Segmentor::~Segmentor() {
    if (preprocesser_ != NULL) {
        delete preprocesser_;
        preprocesser_ = NULL;
    }
    if (ns_dict_ != NULL) {
        delete ns_dict_;
        ns_dict_ = NULL;
    }
    if (idiom_dict_ != NULL) {
        delete idiom_dict_;
        idiom_dict_ = NULL;
    }
    if (nz_dict_ != NULL) {
        delete nz_dict_;
        nz_dict_ = NULL;
    }
    if (ni_dict_ != NULL) {
        delete ni_dict_;
        ni_dict_ = NULL;
    }
    if (noun_dict_ != NULL) {
        delete noun_dict_;
        noun_dict_ = NULL;
    }
    if (adj_dict_ != NULL) {
        delete adj_dict_;
        adj_dict_ = NULL;
    }
    if (verb_dict_ != NULL) {
        delete verb_dict_;
        verb_dict_ = NULL;
    }
    if (vm_dict_ != NULL) {
        delete vm_dict_;
        vm_dict_ = NULL;
    }
    if (y_dict_ != NULL) {
        delete y_dict_;
        y_dict_ = NULL;
    }
    if(user_dict_ != NULL){
        delete user_dict_;
        user_dict_ = NULL;
    }
    if (negword_ != NULL) {
        delete negword_;
        negword_ = NULL;
    }
    if (timeword_ != NULL) {
        delete timeword_;
        timeword_ = NULL;
    }
    if (verbword_ != NULL) {
        delete verbword_;
        verbword_ = NULL;
    }
    if (punctuation_ != NULL) {
        delete punctuation_;
        punctuation_ = NULL;
    }

    if(useFilter_ && filter_ != NULL){
        delete filter_;
        filter_ = NULL;
    }

    if (lf_ != NULL) {
        delete lf_;
    }

    if (cws_decoder_ != NULL) {
        delete cws_decoder_;
    }

    if(cws_model_ != NULL){
        for(int i = 0; i < cws_model_->l_size; i ++){
            if(cws_label_info_) {
                delete[](cws_label_info_[i]);
            }
        }
        delete[] cws_label_info_;
    }

    if (cws_pocs_to_tags_ != NULL){
        for(int i = 1; i < 16; i ++){
            delete[] cws_pocs_to_tags_[i];
        }
        delete[] cws_pocs_to_tags_;
    }

    if (cws_dat_ != NULL) {
        delete cws_dat_;
    }

    if (cws_model_ != NULL) {
        delete cws_model_;
    }

    if (tagging_decoder_ != NULL) {
        delete tagging_decoder_;
    }

    if (tagging_model_ != NULL){
        for(int i = 0; i < tagging_model_->l_size; i ++){
            if(tagging_label_info_) {
                delete[](tagging_label_info_[i]);
            }
        }
        delete[] tagging_label_info_;
    }

    if (tagging_pocs_to_tags_ != NULL){
        for(int i = 1; i < 16; i ++){
            delete[] tagging_pocs_to_tags_[i];
        }
        delete[] tagging_pocs_to_tags_;
    }

    if (tagging_dat_ != NULL) {
        delete tagging_dat_;
    }

    if (tagging_model_ != NULL) {
        delete tagging_model_;
    }
}


NAN_METHOD(Segmentor::New) {
    if (!info.IsConstructCall()) {
        Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
        return;
    }

    // accept a reference or v8:External?
    if (info[0]->IsExternal()) {
        v8::Local<v8::External> ext = info[0].As<v8::External>();
        void* ptr = ext->Value();
        Segmentor* m =  static_cast<Segmentor*>(ptr);
        m->Wrap(info.This());
        info.GetReturnValue().Set(info.This());
        return;
    }

    Segmentor* m = new Segmentor();
    m->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
    return;
}

NAN_METHOD(Segmentor::load_model) {
    if (info.Length() < 1 || !info[0]->IsObject()) {
        Nan::ThrowTypeError("first argument must be a object!");
        return;
    }

    v8::Local<v8::Object> options = info[0].As<v8::Object>();

    // this parameter specifies to convert traditional chinese to simplified chinese
    bool traditional_chinese_to_simplified_chinese = false;
    v8::Local<v8::String> t2s_param = Nan::New("t2s").ToLocalChecked();
    if (options->Has(t2s_param)) {
        v8::Local<v8::Value> param_val = options->Get(t2s_param);
        if (!param_val->IsBoolean()) {
            Nan::ThrowTypeError("t2s parameter must be type of Boolean");
            return;
        }
        traditional_chinese_to_simplified_chinese = param_val->BooleanValue();
    }

    // this parameter specifies to segment text without Part-of-Speech
    bool segment_text_without_part_of_speech = false;
    v8::Local<v8::String> seg_only_param = Nan::New("segOnly").ToLocalChecked();
    if (options->Has(seg_only_param)) {
        v8::Local<v8::Value> param_val = options->Get(seg_only_param);
        if (!param_val->IsBoolean()) {
            Nan::ThrowTypeError("seg_only_param parameter must be type of Boolean");
            return;
        }
        segment_text_without_part_of_speech = param_val->BooleanValue();
    }

    // this parameter specifies to use filter to remove the words that have no much sense, like \"could\"
    bool usefilter = false;
    v8::Local<v8::String> filter_param = Nan::New("filter").ToLocalChecked();
    if (options->Has(filter_param)) {
        v8::Local<v8::Value> param_val = options->Get(filter_param);
        if (!param_val->IsBoolean()) {
            Nan::ThrowTypeError("filter parameter must be type of Boolean");
            return;
        }
        usefilter = param_val->BooleanValue();
    }

    // this parameter specifies tagsign delimeter between words and POS tags. Default is _"
    Character delimeter = '_';
    v8::Local<v8::String> delimeter_param = Nan::New("delimeter").ToLocalChecked();
    if (options->Has(delimeter_param)) {
        v8::Local<v8::Value> param_val = options->Get(delimeter_param);
        if (!param_val->IsString()) {
            Nan::ThrowTypeError("delimeter parameter must be type of string");
            return;
        }
        delimeter = (*v8::String::Utf8Value(param_val))[0];
    }

    // Use the words in the userword.txt as a dictionary and the words will labled as \"uw\""
    std::string userword = "";
    v8::Local<v8::String> userword_param = Nan::New("userword").ToLocalChecked();
    if (options->Has(userword_param)) {
        v8::Local<v8::Value> param_val = options->Get(userword_param);
        userword = *v8::String::Utf8Value(param_val);
    }

    // this parameter specifies dir is the directory that containts all the model file. Default is \"models/\"
    std::string model_dir = "";
    bool lite_model = true;
    v8::Local<v8::String> model_dir_param = Nan::New("modelDir").ToLocalChecked();
    if (options->Has(model_dir_param)) {
        v8::Local<v8::Value> param_val = options->Get(model_dir_param);
        model_dir = *v8::String::Utf8Value(param_val);
        lite_model = false;
    } else {
        model_dir = "./models/";
    }

    Segmentor* segmentor = Nan::ObjectWrap::Unwrap<Segmentor>(info.Holder());

    // begin process
    segmentor->separator_ = delimeter;

    segmentor->useT2S_ = traditional_chinese_to_simplified_chinese;
    segmentor->seg_only_ = segment_text_without_part_of_speech;
    segmentor->useFilter_ = usefilter;
    segmentor->use_second_ = false;
    segmentor->userword_ = userword;

    std::string prefix = model_dir;

    // std::cout << "setting up cws decoder ...\n";
    segmentor->cws_decoder_ = new TaggingDecoder();
    segmentor->cws_model_ = new permm::Model((prefix + "cws_model.bin").c_str());
    segmentor->cws_dat_ = new DAT((prefix + "cws_dat.bin").c_str());
    segmentor->cws_label_info_ = new char*[segmentor->cws_model_->l_size];
    segmentor->cws_pocs_to_tags_ = new int*[16];
    get_label_info((prefix + "cws_label.txt").c_str(), segmentor->cws_label_info_, segmentor->cws_pocs_to_tags_);
    segmentor->cws_decoder_->init(segmentor->cws_model_, segmentor->cws_dat_, segmentor->cws_label_info_, segmentor->cws_pocs_to_tags_);
    segmentor->cws_decoder_->set_label_trans();

    if (segmentor->seg_only_) {
        segmentor->cws_decoder_->threshold = 0;
    } else {
        segmentor->cws_decoder_->threshold = 15000;
    }

    // std::cout << "setting up tagging decoder ...\n";
    if (!segmentor->seg_only_) {
        segmentor->tagging_decoder_ = new TaggingDecoder();
        segmentor->tagging_decoder_->separator = segmentor->separator_;
        if (segmentor->use_second_) {
            segmentor->tagging_decoder_->threshold = 10000;
        } else {
            segmentor->tagging_decoder_->threshold = 0;
        }

        segmentor->tagging_model_ = new permm::Model((prefix + "model_c_model.bin").c_str());
        segmentor->tagging_dat_ = new DAT((prefix + "model_c_dat.bin").c_str());
        segmentor->tagging_label_info_ = new char*[segmentor->tagging_model_->l_size];
        segmentor->tagging_pocs_to_tags_ = new int*[16];

        get_label_info((prefix + "model_c_label.txt").c_str(), segmentor->tagging_label_info_, segmentor->tagging_pocs_to_tags_);
        segmentor->tagging_decoder_->init(segmentor->tagging_model_, segmentor->tagging_dat_, segmentor->tagging_label_info_, segmentor->tagging_pocs_to_tags_);
        segmentor->tagging_decoder_->set_label_trans();
    }

    // std::cout << "setting up dictionaries\n";

    segmentor->lf_ = new thulac::hypergraph::LatticeFeature();
    if (is_file_exist((prefix + "sgT.dat").c_str()) && is_file_exist((prefix + "sgW.dat").c_str()) ) {
        segmentor->sogout_ = new DAT((prefix + "sgT.dat").c_str());
        segmentor->lf_->node_features.push_back(new thulac::hypergraph::SogouTFeature(segmentor->sogout_));

        std::vector<std::string> n_gram_model;
        std::vector<std::string> dictionaries;
        dictionaries.push_back(prefix + "sgW.dat");
        for(size_t i = 0; i < dictionaries.size(); ++i) {
            segmentor->lf_->node_features.push_back(new thulac::hypergraph::DictNodeFeature(new DAT(dictionaries[i].c_str())));
        }
    }

    segmentor->lf_->filename= prefix + "model_w";
    segmentor->lf_->load();

    segmentor->decoder_.features.push_back(segmentor->lf_);

    // std::cout << "setup preprocesser ...\n";
    segmentor->preprocesser_ = new Preprocesser();
    segmentor->preprocesser_->setT2SMap((prefix + "t2s.dat").c_str());

    segmentor->ns_dict_ = new Postprocesser((prefix + "ns.dat").c_str(), "ns", false);
    segmentor->idiom_dict_ = new Postprocesser((prefix + "idiom.dat").c_str(), "i", false);
    if (is_file_exist((prefix + "nz.dat").c_str())) {
        segmentor->nz_dict_ = new Postprocesser((prefix + "nz.dat").c_str(), "nz", false);
    }
    if (is_file_exist((prefix + "ni.dat").c_str())) {
        segmentor->ni_dict_ = new Postprocesser((prefix + "ni.dat").c_str(), "ni", false);
    }
    if (is_file_exist((prefix + "noun.dat").c_str())) {
        segmentor->noun_dict_ = new Postprocesser((prefix + "noun.dat").c_str(), "n", false);
    }

    if (is_file_exist((prefix + "adj.dat").c_str())) {
        segmentor->adj_dict_ = new Postprocesser((prefix + "adj.dat").c_str(), "a", false);
    }

    if (is_file_exist((prefix + "verb.dat").c_str())) {
        segmentor->verb_dict_ = new Postprocesser((prefix + "verb.dat").c_str(), "v", false);
    }

    if (is_file_exist((prefix + "vm.dat").c_str())) {
        segmentor->vm_dict_ = new Postprocesser((prefix + "vm.dat").c_str(), "vm", false);
    }

    if (is_file_exist((prefix + "y.dat").c_str())) {
        segmentor->y_dict_ = new Postprocesser((prefix + "y.dat").c_str(), "y", false);
    }

    if (userword != ""){
        segmentor->user_dict_ = new Postprocesser(userword.c_str(), "uw", true);
    }

    segmentor->punctuation_ = new Punctuation((prefix + "singlepun.dat").c_str());

    segmentor->negword_ = new NegWord((prefix + "neg.dat").c_str());
    segmentor->timeword_ = new TimeWord();
    segmentor->verbword_ = new VerbWord((prefix + "vM.dat").c_str(), (prefix + "vD.dat").c_str());

    segmentor->filter_ = NULL;
    if (segmentor->useFilter_){
        segmentor->filter_ = new Filter((prefix + "xu.dat").c_str(), (prefix + "time.dat").c_str());
    }

    // std::cout << "Done loading\n";

    info.GetReturnValue().Set(Nan::New(true));
}

NAN_METHOD(Segmentor::predict) {
    if (info.Length() != 1) {
        Nan::ThrowError("Need the model prefix");
        return;
    }
    Chinese_Charset_Conv conv;

    Segmentor* segmentor = Nan::ObjectWrap::Unwrap<Segmentor>(info.Holder());
    std::string ori = *v8::String::Utf8Value(info[0]->ToString());

    POCGraph poc_cands;
    POCGraph new_poc_cands;
    thulac::RawSentence oriRaw;
    thulac::RawSentence raw;
    thulac::RawSentence tRaw;
    thulac::SegmentedSentence segged;
    thulac::TaggedSentence tagged;

    const int BYTES_LEN = 10000;
    char* input = new char[BYTES_LEN];
    char* output = new char[BYTES_LEN];

    std::string result = "";

    int codetype = -1;
    std::ostringstream ost;
    bool containtsT = false;

    strcpy(input, ori.c_str());
    size_t in_left = ori.length();
    size_t out_left = BYTES_LEN;

    // decide codetype
    codetype = conv.conv(input, in_left, output, out_left);
    if (codetype >= 0) {
        int outlen = BYTES_LEN - out_left;
        thulac::get_raw(oriRaw, output, outlen);
    } else {
        std::cout << "File should be encoded in UTF8 or GBK\n";
        // return ...
        return;
    }

    // std::cout << "code type: " << codetype << std::endl;
    // get raw
    if (codetype == 0) {
        thulac::get_raw(oriRaw, input, in_left);
    } else {
        size_t out_left = BYTES_LEN;
        codetype = conv.conv(input, in_left, output, out_left, codetype);
        int outlen = BYTES_LEN - out_left;
        thulac::get_raw(oriRaw, output, outlen);
    }

    // process tradition chinese
    if (segmentor->preprocesser_->containsT(oriRaw)) {
        // std::cout << "process traditional chinese\n";
        segmentor->preprocesser_->clean(oriRaw, tRaw, poc_cands);
        segmentor->preprocesser_->T2S(tRaw, raw);
        containtsT = true;
    } else {
        segmentor->preprocesser_->clean(oriRaw, raw, poc_cands);
    }

    // segmentation
    if(raw.size()) {
        // std::cout << "raw size > 0\n";
        segmentor->cws_decoder_->segment(raw, poc_cands, new_poc_cands);
        if (segmentor->seg_only_) {
            // std::cout << "segment only...\n";
            segmentor->cws_decoder_->segment(raw, poc_cands, new_poc_cands);
            segmentor->cws_decoder_->get_seg_result(segged);
            segmentor->ns_dict_->adjust(segged);
            segmentor->idiom_dict_->adjust(segged);
            if (segmentor->nz_dict_) {
                segmentor->nz_dict_->adjust(segged);
            }
            if (segmentor->noun_dict_) {
                segmentor->noun_dict_->adjust(segged);
            }

            if (segmentor->user_dict_){
                segmentor->user_dict_->adjust(segged);
            }
            segmentor->punctuation_->adjust(segged);
            segmentor->timeword_->adjust(segged);
            if (segmentor->useFilter_){
                segmentor->filter_->adjust(segged);
            }

            if (codetype == 0) {
                // std::cout << "codetype == 0...\n";
                for(size_t j = 0; j < segged.size(); j++){
                    if (j != 0) {
                        ost << " ";
                    }
                    ost << segged[j];
                }
                //ost.str("");
                result = ost.str();
            } else {
                for(size_t j = 0; j < segged.size(); j++){
                    if (j != 0) {
                      ost << " ";
                    }
                    ost << segged[j];
                }

                std::string str = ost.str();
                strcpy(input, str.c_str());
                size_t in_left = str.size();
                size_t out_left = BYTES_LEN;
                codetype = conv.invert_conv(input, in_left, output, out_left, codetype);
                //int outlen = BYTES_LEN - out_left;

                // ost.str("");
                result = ost.str();
            }
        } else {
            // std::cout << "raw size < 0\n";
            if (segmentor->use_second_) {
                Lattice lattice;
                hypergraph::Graph graph;
                segmentor->tagging_decoder_->segment(raw, new_poc_cands, lattice);
                hypergraph::lattice_to_graph(lattice, graph);
                segmentor->decoder_.decode(graph);
                hypergraph::graph_to_lattice(graph,lattice,1);
                lattice_to_sentence(lattice,tagged, (char)segmentor->separator_);
            } else {
                segmentor->tagging_decoder_->segment(raw, new_poc_cands, tagged);
            }

            segmentor->ns_dict_->adjust(tagged);
            segmentor->idiom_dict_->adjust(tagged);
            if (segmentor->nz_dict_) {
                segmentor->nz_dict_->adjust(segged);
            }
            if (segmentor->ni_dict_) {
                segmentor->ni_dict_->adjust(tagged);
            }
            if (segmentor->noun_dict_) {
                segmentor->noun_dict_->adjust(segged);
            }
            if (segmentor->adj_dict_) {
                segmentor->adj_dict_->adjust(tagged);
            }
            if (segmentor->verb_dict_) {
                segmentor->verb_dict_->adjust(tagged);
            }
            if (segmentor->vm_dict_) {
                segmentor->vm_dict_->adjustSame(tagged);
            }
            if (segmentor->y_dict_) {
                segmentor->y_dict_->adjustSame(tagged);
            }

            if (segmentor->user_dict_) {
                segmentor->user_dict_->adjust(tagged);
            }
            segmentor->punctuation_->adjust(tagged);
            segmentor->timeword_->adjustDouble(tagged);
            segmentor->negword_->adjust(tagged);
            segmentor->verbword_->adjust(tagged);
            if (segmentor->useFilter_) {
                segmentor->filter_->adjust(tagged);
            }

            if (containtsT && !segmentor->useT2S_) {
                segmentor->preprocesser_->S2T(tagged, tRaw);
            }

            if (codetype == 0) {
                ost << tagged;
                result = ost.str();
            } else {
                ost << tagged;
                std::string str = ost.str();
                strcpy(input, str.c_str());
                size_t in_left = str.size();
                size_t out_left = BYTES_LEN;
                codetype = conv.invert_conv(input, in_left, output, out_left, codetype);
                //int outlen=BYTES_LEN - out_left;
                // result = std::string(output, outlen);
                ost.str("");
                result = ost.str();
            }
        }
        // segmentor->tagging_decoder_->output_sentence(output);
    }

    delete[] input;
    delete [] output;
    info.GetReturnValue().Set(Nan::New<v8::String>(result).ToLocalChecked());
}


#endif
