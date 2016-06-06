#include <node.h>

#include "wrapper.hpp"

static void initAll(v8::Handle<v8::Object> target) {
    Nan::HandleScope scope;
    Segmentor::Initialize(target);
}

NODE_MODULE(Segmentor, initAll)

