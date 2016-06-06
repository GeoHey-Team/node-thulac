#ifndef __thulac__wrapper__hh__
#define __thulac__wrapper__hh__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wshadow"
#include <nan.h>
#pragma GCC diagnostic pop

#include <string>
#include <memory>

#define TOSTR(obj) (*v8::String::Utf8Value((obj)->ToString()))

#include "cb_tagging_decoder.h"
#include "preprocess.h"
using namespace thulac;

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
    permm::Model* tagging_model_;
    DAT* tagging_dat_;
    char** tagging_label_info_;
    int** tagging_pocs_to_tags_;
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
    tagging_decoder_ = new TaggingDecoder();
    tagging_decoder_->separator = '_';
    tagging_decoder_->threshold = 0;
    tagging_model_ = NULL;
    tagging_label_info_ = NULL;
    tagging_pocs_to_tags_ = NULL;
    tagging_dat_ = NULL;
}

Segmentor::~Segmentor() {
    delete tagging_decoder_;
    if(tagging_model_) {
        for(int i = 0; i < tagging_model_->l_size; i ++) {
            if (tagging_label_info_)
                delete[] (tagging_label_info_[i]);
        }

    }
    delete[] tagging_label_info_;

    if (tagging_pocs_to_tags_) {
        for(int i = 1; i < 16; i ++){
            delete[] (tagging_pocs_to_tags_[i]);
        }
    }
    delete[] tagging_pocs_to_tags_;

    delete tagging_dat_;
    if (tagging_model_)
        delete tagging_model_;
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
    if (info.Length() != 1) {
        Nan::ThrowError("Need the model prefix");
        return;
    }

    Segmentor* segmentor = Nan::ObjectWrap::Unwrap<Segmentor>(info.Holder());

    std::string prefix = *v8::String::Utf8Value(info[0]->ToString());

    segmentor->tagging_model_ = new permm::Model((prefix + "_model.bin").c_str());
    segmentor->tagging_dat_ = new DAT((prefix + "_dat.bin").c_str());
    segmentor->tagging_label_info_ = new char*[segmentor->tagging_model_->l_size];
    segmentor->tagging_pocs_to_tags_ = new int*[16];

    get_label_info((prefix + "_label.txt").c_str(), segmentor->tagging_label_info_, segmentor->tagging_pocs_to_tags_);
    segmentor->tagging_decoder_->init(segmentor->tagging_model_, segmentor->tagging_dat_, segmentor->tagging_label_info_, segmentor->tagging_pocs_to_tags_);
    segmentor->tagging_decoder_->set_label_trans();

    info.GetReturnValue().Set(Nan::New(true));
}



NAN_METHOD(Segmentor::predict) {
    if (info.Length() != 1) {
        Nan::ThrowError("Need the model prefix");
        return;
    }

    Segmentor* segmentor = Nan::ObjectWrap::Unwrap<Segmentor>(info.Holder());
    std::string ori = *v8::String::Utf8Value(info[0]->ToString());

    Preprocesser* preprocesser = new Preprocesser();

    POCGraph poc_cands;
    thulac::RawSentence oriRaw;
    thulac::RawSentence raw;
    const int BYTES_LEN = 10000;
    char* input = new char[BYTES_LEN];
    char* output = new char[BYTES_LEN];

    strcpy(input,ori.c_str());
    size_t in_left=ori.length();
    thulac::get_raw(oriRaw, input, in_left);
    preprocesser->clean(oriRaw, raw, poc_cands);

    if(raw.size()){
        segmentor->tagging_decoder_->segment(raw);
        segmentor->tagging_decoder_->output_sentence(output);
    }

    std::string result = output;
    delete[] input;
    delete [] output;
    info.GetReturnValue().Set(Nan::New<v8::String>(result).ToLocalChecked());
}


#endif
