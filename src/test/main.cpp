#include "util/json/json.h"
#include "glog/logging.h"
int main(int agrv, char** agrc)
{
    Json_u::Value cc;
    cc["a"]=1;
    cc["b"]=2;
    cc["c"]=3;
    Json_u::FastWriter w;
    LOG(INFO)<<w.write(cc);
    return 0;
}
