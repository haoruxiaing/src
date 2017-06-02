#include "common/emoj.h"
#include <map>
#include <string>
#include <locale>
#include <iostream>
#include <cstdlib>
#include <clocale>
using namespace std;

static map<string, string> emojset;

void CEmojConvert::init()
{
    emojset["\xe2\x98\x80"] = "e29880";
    emojset["\xe2\x98\x81"] = "e29881";
    emojset["\xe2\x98\x94"] = "e29894";
    emojset["\xe2\x9b\x84"] = "e29b84";
    emojset["\xe2\x9a\xa1"] = "e29aa1";
    emojset["\xf0\x9f\x8c\x80"] = "f09f8c80";
    emojset["\xf0\x9f\x8c\x81"] = "f09f8c81";
    emojset["\xf0\x9f\x8c\x82"] = "f09f8c82";
    emojset["\xf0\x9f\x8c\x83"] = "f09f8c83";
    emojset["\xf0\x9f\x8c\x84"] = "f09f8c84";
    emojset["\xf0\x9f\x8c\x85"] = "f09f8c85";
    emojset["\xf0\x9f\x8c\x86"] = "f09f8c86";
    emojset["\xf0\x9f\x8c\x87"] = "f09f8c87";
    emojset["\xf0\x9f\x8c\x88"] = "f09f8c88";
    emojset["\xe2\x9d\x84"] = "e29d84";
    emojset["\xe2\x9b\x85"] = "e29b85";
    emojset["\xf0\x9f\x8c\x89"] = "f09f8c89";
    emojset["\xf0\x9f\x8c\x8a"] = "f09f8c8a";
    emojset["\xf0\x9f\x8c\x8b"] = "f09f8c8b";
    emojset["\xf0\x9f\x8c\x8c"] = "f09f8c8c";
    emojset["\xf0\x9f\x8c\x8f"] = "f09f8c8f";
    emojset["\xf0\x9f\x8c\x91"] = "f09f8c91";
    emojset["\xf0\x9f\x8c\x94"] = "f09f8c94";
    emojset["\xf0\x9f\x8c\x93"] = "f09f8c93";
    emojset["\xf0\x9f\x8c\x99"] = "f09f8c99";
    emojset["\xf0\x9f\x8c\x95"] = "f09f8c95";
    emojset["\xf0\x9f\x8c\x9b"] = "f09f8c9b";
    emojset["\xf0\x9f\x8c\x9f"] = "f09f8c9f";
    emojset["\xf0\x9f\x8c\xa0"] = "f09f8ca0";
    emojset["\xf0\x9f\x95\x90"] = "f09f9590";
    emojset["\xf0\x9f\x95\x91"] = "f09f9591";
    emojset["\xf0\x9f\x95\x92"] = "f09f9592";
    emojset["\xf0\x9f\x95\x93"] = "f09f9593";
    emojset["\xf0\x9f\x95\x94"] = "f09f9594";
    emojset["\xf0\x9f\x95\x95"] = "f09f9595";
    emojset["\xf0\x9f\x95\x96"] = "f09f9596";
    emojset["\xf0\x9f\x95\x97"] = "f09f9597";
    emojset["\xf0\x9f\x95\x98"] = "f09f9598";
    emojset["\xf0\x9f\x95\x99"] = "f09f9599";
    emojset["\xf0\x9f\x95\x9a"] = "f09f959a";
    emojset["\xf0\x9f\x95\x9b"] = "f09f959b";
    emojset["\xe2\x8c\x9a"] = "e28c9a";
    emojset["\xe2\x8c\x9b"] = "e28c9b";
    emojset["\xe2\x8f\xb0"] = "e28fb0";
    emojset["\xe2\x8f\xb3"] = "e28fb3";
    emojset["\xe2\x99\x88"] = "e29988";
    emojset["\xe2\x99\x89"] = "e29989";
    emojset["\xe2\x99\x8a"] = "e2998a";
    emojset["\xe2\x99\x8b"] = "e2998b";
    emojset["\xe2\x99\x8c"] = "e2998c";
    emojset["\xe2\x99\x8d"] = "e2998d";
    emojset["\xe2\x99\x8e"] = "e2998e";
    emojset["\xe2\x99\x8f"] = "e2998f";
    emojset["\xe2\x99\x90"] = "e29990";
    emojset["\xe2\x99\x91"] = "e29991";
    emojset["\xe2\x99\x92"] = "e29992";
    emojset["\xe2\x99\x93"] = "e29993";
    emojset["\xe2\x9b\x8e"] = "e29b8e";
    emojset["\xf0\x9f\x8d\x80"] = "f09f8d80";
    emojset["\xf0\x9f\x8c\xb7"] = "f09f8cb7";
    emojset["\xf0\x9f\x8c\xb1"] = "f09f8cb1";
    emojset["\xf0\x9f\x8d\x81"] = "f09f8d81";
    emojset["\xf0\x9f\x8c\xb8"] = "f09f8cb8";
    emojset["\xf0\x9f\x8c\xb9"] = "f09f8cb9";
    emojset["\xf0\x9f\x8d\x82"] = "f09f8d82";
    emojset["\xf0\x9f\x8d\x83"] = "f09f8d83";
    emojset["\xf0\x9f\x8c\xba"] = "f09f8cba";
    emojset["\xf0\x9f\x8c\xbb"] = "f09f8cbb";
    emojset["\xf0\x9f\x8c\xb4"] = "f09f8cb4";
    emojset["\xf0\x9f\x8c\xb5"] = "f09f8cb5";
    emojset["\xf0\x9f\x8c\xbe"] = "f09f8cbe";
    emojset["\xf0\x9f\x8c\xbd"] = "f09f8cbd";
    emojset["\xf0\x9f\x8d\x84"] = "f09f8d84";
    emojset["\xf0\x9f\x8c\xb0"] = "f09f8cb0";
    emojset["\xf0\x9f\x8c\xbc"] = "f09f8cbc";
    emojset["\xf0\x9f\x8c\xbf"] = "f09f8cbf";
    emojset["\xf0\x9f\x8d\x92"] = "f09f8d92";
    emojset["\xf0\x9f\x8d\x8c"] = "f09f8d8c";
    emojset["\xf0\x9f\x8d\x8e"] = "f09f8d8e";
    emojset["\xf0\x9f\x8d\x8a"] = "f09f8d8a";
    emojset["\xf0\x9f\x8d\x93"] = "f09f8d93";
    emojset["\xf0\x9f\x8d\x89"] = "f09f8d89";
    emojset["\xf0\x9f\x8d\x85"] = "f09f8d85";
    emojset["\xf0\x9f\x8d\x86"] = "f09f8d86";
    emojset["\xf0\x9f\x8d\x88"] = "f09f8d88";
    emojset["\xf0\x9f\x8d\x8d"] = "f09f8d8d";
    emojset["\xf0\x9f\x8d\x87"] = "f09f8d87";
    emojset["\xf0\x9f\x8d\x91"] = "f09f8d91";
    emojset["\xf0\x9f\x8d\x8f"] = "f09f8d8f";
    emojset["\xf0\x9f\x91\x80"] = "f09f9180";
    emojset["\xf0\x9f\x91\x82"] = "f09f9182";
    emojset["\xf0\x9f\x91\x83"] = "f09f9183";
    emojset["\xf0\x9f\x91\x84"] = "f09f9184";
    emojset["\xf0\x9f\x91\x85"] = "f09f9185";
    emojset["\xf0\x9f\x92\x84"] = "f09f9284";
    emojset["\xf0\x9f\x92\x85"] = "f09f9285";
    emojset["\xf0\x9f\x92\x86"] = "f09f9286";
    emojset["\xf0\x9f\x92\x87"] = "f09f9287";
    emojset["\xf0\x9f\x92\x88"] = "f09f9288";
    emojset["\xf0\x9f\x91\xa4"] = "f09f91a4";
    emojset["\xf0\x9f\x91\xa6"] = "f09f91a6";
    emojset["\xf0\x9f\x91\xa7"] = "f09f91a7";
    emojset["\xf0\x9f\x91\xa8"] = "f09f91a8";
    emojset["\xf0\x9f\x91\xa9"] = "f09f91a9";
    emojset["\xf0\x9f\x91\xaa"] = "f09f91aa";
    emojset["\xf0\x9f\x91\xab"] = "f09f91ab";
    emojset["\xf0\x9f\x91\xae"] = "f09f91ae";
    emojset["\xf0\x9f\x91\xaf"] = "f09f91af";
    emojset["\xf0\x9f\x91\xb0"] = "f09f91b0";
    emojset["\xf0\x9f\x91\xb1"] = "f09f91b1";
    emojset["\xf0\x9f\x91\xb2"] = "f09f91b2";
    emojset["\xf0\x9f\x91\xb3"] = "f09f91b3";
    emojset["\xf0\x9f\x91\xb4"] = "f09f91b4";
    emojset["\xf0\x9f\x91\xb5"] = "f09f91b5";
    emojset["\xf0\x9f\x91\xb6"] = "f09f91b6";
    emojset["\xf0\x9f\x91\xb7"] = "f09f91b7";
    emojset["\xf0\x9f\x91\xb8"] = "f09f91b8";
    emojset["\xf0\x9f\x91\xb9"] = "f09f91b9";
    emojset["\xf0\x9f\x91\xba"] = "f09f91ba";
    emojset["\xf0\x9f\x91\xbb"] = "f09f91bb";
    emojset["\xf0\x9f\x91\xbc"] = "f09f91bc";
    emojset["\xf0\x9f\x91\xbd"] = "f09f91bd";
    emojset["\xf0\x9f\x91\xbe"] = "f09f91be";
    emojset["\xf0\x9f\x91\xbf"] = "f09f91bf";
    emojset["\xf0\x9f\x92\x80"] = "f09f9280";
    emojset["\xf0\x9f\x92\x81"] = "f09f9281";
    emojset["\xf0\x9f\x92\x82"] = "f09f9282";
    emojset["\xf0\x9f\x92\x83"] = "f09f9283";
    emojset["\xf0\x9f\x90\x8c"] = "f09f908c";
    emojset["\xf0\x9f\x90\x8d"] = "f09f908d";
    emojset["\xf0\x9f\x90\x8e"] = "f09f908e";
    emojset["\xf0\x9f\x90\x94"] = "f09f9094";
    emojset["\xf0\x9f\x90\x97"] = "f09f9097";
    emojset["\xf0\x9f\x90\xab"] = "f09f90ab";
    emojset["\xf0\x9f\x90\x98"] = "f09f9098";
    emojset["\xf0\x9f\x90\xa8"] = "f09f90a8";
    emojset["\xf0\x9f\x90\x92"] = "f09f9092";
    emojset["\xf0\x9f\x90\x91"] = "f09f9091";
    emojset["\xf0\x9f\x90\x99"] = "f09f9099";
    emojset["\xf0\x9f\x90\x9a"] = "f09f909a";
    emojset["\xf0\x9f\x90\x9b"] = "f09f909b";
    emojset["\xf0\x9f\x90\x9c"] = "f09f909c";
    emojset["\xf0\x9f\x90\x9d"] = "f09f909d";
    emojset["\xf0\x9f\x90\x9e"] = "f09f909e";
    emojset["\xf0\x9f\x90\xa0"] = "f09f90a0";
    emojset["\xf0\x9f\x90\xa1"] = "f09f90a1";
    emojset["\xf0\x9f\x90\xa2"] = "f09f90a2";
    emojset["\xf0\x9f\x90\xa4"] = "f09f90a4";
    emojset["\xf0\x9f\x90\xa5"] = "f09f90a5";
    emojset["\xf0\x9f\x90\xa6"] = "f09f90a6";
    emojset["\xf0\x9f\x90\xa3"] = "f09f90a3";
    emojset["\xf0\x9f\x90\xa7"] = "f09f90a7";
    emojset["\xf0\x9f\x90\xa9"] = "f09f90a9";
    emojset["\xf0\x9f\x90\x9f"] = "f09f909f";
    emojset["\xf0\x9f\x90\xac"] = "f09f90ac";
    emojset["\xf0\x9f\x90\xad"] = "f09f90ad";
    emojset["\xf0\x9f\x90\xaf"] = "f09f90af";
    emojset["\xf0\x9f\x90\xb1"] = "f09f90b1";
    emojset["\xf0\x9f\x90\xb3"] = "f09f90b3";
    emojset["\xf0\x9f\x90\xb4"] = "f09f90b4";
    emojset["\xf0\x9f\x90\xb5"] = "f09f90b5";
    emojset["\xf0\x9f\x90\xb6"] = "f09f90b6";
    emojset["\xf0\x9f\x90\xb7"] = "f09f90b7";
    emojset["\xf0\x9f\x90\xbb"] = "f09f90bb";
    emojset["\xf0\x9f\x90\xb9"] = "f09f90b9";
    emojset["\xf0\x9f\x90\xba"] = "f09f90ba";
    emojset["\xf0\x9f\x90\xae"] = "f09f90ae";
    emojset["\xf0\x9f\x90\xb0"] = "f09f90b0";
    emojset["\xf0\x9f\x90\xb8"] = "f09f90b8";
    emojset["\xf0\x9f\x90\xbe"] = "f09f90be";
    emojset["\xf0\x9f\x90\xb2"] = "f09f90b2";
    emojset["\xf0\x9f\x90\xbc"] = "f09f90bc";
    emojset["\xf0\x9f\x90\xbd"] = "f09f90bd";
    emojset["\xf0\x9f\x98\xa0"] = "f09f98a0";
    emojset["\xf0\x9f\x98\xa9"] = "f09f98a9";
    emojset["\xf0\x9f\x98\xb2"] = "f09f98b2";
    emojset["\xf0\x9f\x98\x9e"] = "f09f989e";
    emojset["\xf0\x9f\x98\xb5"] = "f09f98b5";
    emojset["\xf0\x9f\x98\xb0"] = "f09f98b0";
    emojset["\xf0\x9f\x98\x92"] = "f09f9892";
    emojset["\xf0\x9f\x98\x8d"] = "f09f988d";
    emojset["\xf0\x9f\x98\xa4"] = "f09f98a4";
    emojset["\xf0\x9f\x98\x9c"] = "f09f989c";
    emojset["\xf0\x9f\x98\x9d"] = "f09f989d";
    emojset["\xf0\x9f\x98\x8b"] = "f09f988b";
    emojset["\xf0\x9f\x98\x98"] = "f09f9898";
    emojset["\xf0\x9f\x98\x9a"] = "f09f989a";
    emojset["\xf0\x9f\x98\xb7"] = "f09f98b7";
    emojset["\xf0\x9f\x98\xb3"] = "f09f98b3";
    emojset["\xf0\x9f\x98\x83"] = "f09f9883";
    emojset["\xf0\x9f\x98\x85"] = "f09f9885";
    emojset["\xf0\x9f\x98\x86"] = "f09f9886";
    emojset["\xf0\x9f\x98\x81"] = "f09f9881";
    emojset["\xf0\x9f\x98\x82"] = "f09f9882";
    emojset["\xf0\x9f\x98\x8a"] = "f09f988a";
    emojset["\xe2\x98\xba"] = "e298ba";
    emojset["\xf0\x9f\x98\x84"] = "f09f9884";
    emojset["\xf0\x9f\x98\xa2"] = "f09f98a2";
    emojset["\xf0\x9f\x98\xad"] = "f09f98ad";
    emojset["\xf0\x9f\x98\xa8"] = "f09f98a8";
    emojset["\xf0\x9f\x98\xa3"] = "f09f98a3";
    emojset["\xf0\x9f\x98\xa1"] = "f09f98a1";
    emojset["\xf0\x9f\x98\x8c"] = "f09f988c";
    emojset["\xf0\x9f\x98\x96"] = "f09f9896";
    emojset["\xf0\x9f\x98\x94"] = "f09f9894";
    emojset["\xf0\x9f\x98\xb1"] = "f09f98b1";
    emojset["\xf0\x9f\x98\xaa"] = "f09f98aa";
    emojset["\xf0\x9f\x98\x8f"] = "f09f988f";
    emojset["\xf0\x9f\x98\x93"] = "f09f9893";
    emojset["\xf0\x9f\x98\xa5"] = "f09f98a5";
    emojset["\xf0\x9f\x98\xab"] = "f09f98ab";
    emojset["\xf0\x9f\x98\x89"] = "f09f9889";
    emojset["\xf0\x9f\x98\xba"] = "f09f98ba";
    emojset["\xf0\x9f\x98\xb8"] = "f09f98b8";
    emojset["\xf0\x9f\x98\xb9"] = "f09f98b9";
    emojset["\xf0\x9f\x98\xbd"] = "f09f98bd";
    emojset["\xf0\x9f\x98\xbb"] = "f09f98bb";
    emojset["\xf0\x9f\x98\xbf"] = "f09f98bf";
    emojset["\xf0\x9f\x98\xbe"] = "f09f98be";
    emojset["\xf0\x9f\x98\xbc"] = "f09f98bc";
    emojset["\xf0\x9f\x99\x80"] = "f09f9980";
    emojset["\xf0\x9f\x99\x85"] = "f09f9985";
    emojset["\xf0\x9f\x99\x86"] = "f09f9986";
    emojset["\xf0\x9f\x99\x87"] = "f09f9987";
    emojset["\xf0\x9f\x99\x88"] = "f09f9988";
    emojset["\xf0\x9f\x99\x8a"] = "f09f998a";
    emojset["\xf0\x9f\x99\x89"] = "f09f9989";
    emojset["\xf0\x9f\x99\x8b"] = "f09f998b";
    emojset["\xf0\x9f\x99\x8c"] = "f09f998c";
    emojset["\xf0\x9f\x99\x8d"] = "f09f998d";
    emojset["\xf0\x9f\x99\x8e"] = "f09f998e";
    emojset["\xf0\x9f\x99\x8f"] = "f09f998f";
    emojset["\xf0\x9f\x8f\xa0"] = "f09f8fa0";
    emojset["\xf0\x9f\x8f\xa1"] = "f09f8fa1";
    emojset["\xf0\x9f\x8f\xa2"] = "f09f8fa2";
    emojset["\xf0\x9f\x8f\xa3"] = "f09f8fa3";
    emojset["\xf0\x9f\x8f\xa5"] = "f09f8fa5";
    emojset["\xf0\x9f\x8f\xa6"] = "f09f8fa6";
    emojset["\xf0\x9f\x8f\xa7"] = "f09f8fa7";
    emojset["\xf0\x9f\x8f\xa8"] = "f09f8fa8";
    emojset["\xf0\x9f\x8f\xa9"] = "f09f8fa9";
    emojset["\xf0\x9f\x8f\xaa"] = "f09f8faa";
    emojset["\xf0\x9f\x8f\xab"] = "f09f8fab";
    emojset["\xe2\x9b\xaa"] = "e29baa";
    emojset["\xe2\x9b\xb2"] = "e29bb2";
    emojset["\xf0\x9f\x8f\xac"] = "f09f8fac";
    emojset["\xf0\x9f\x8f\xaf"] = "f09f8faf";
    emojset["\xf0\x9f\x8f\xb0"] = "f09f8fb0";
    emojset["\xf0\x9f\x8f\xad"] = "f09f8fad";
    emojset["\xe2\x9a\x93"] = "e29a93";
    emojset["\xf0\x9f\x8f\xae"] = "f09f8fae";
    emojset["\xf0\x9f\x97\xbb"] = "f09f97bb";
    emojset["\xf0\x9f\x97\xbc"] = "f09f97bc";
    emojset["\xf0\x9f\x97\xbd"] = "f09f97bd";
    emojset["\xf0\x9f\x97\xbe"] = "f09f97be";
    emojset["\xf0\x9f\x97\xbf"] = "f09f97bf";
    emojset["\xf0\x9f\x91\x9e"] = "f09f919e";
    emojset["\xf0\x9f\x91\x9f"] = "f09f919f";
    emojset["\xf0\x9f\x91\xa0"] = "f09f91a0";
    emojset["\xf0\x9f\x91\xa1"] = "f09f91a1";
    emojset["\xf0\x9f\x91\xa2"] = "f09f91a2";
    emojset["\xf0\x9f\x91\xa3"] = "f09f91a3";
    emojset["\xf0\x9f\x91\x93"] = "f09f9193";
    emojset["\xf0\x9f\x91\x95"] = "f09f9195";
    emojset["\xf0\x9f\x91\x96"] = "f09f9196";
    emojset["\xf0\x9f\x91\x91"] = "f09f9191";
    emojset["\xf0\x9f\x91\x94"] = "f09f9194";
    emojset["\xf0\x9f\x91\x92"] = "f09f9192";
    emojset["\xf0\x9f\x91\x97"] = "f09f9197";
    emojset["\xf0\x9f\x91\x98"] = "f09f9198";
    emojset["\xf0\x9f\x91\x99"] = "f09f9199";
    emojset["\xf0\x9f\x91\x9a"] = "f09f919a";
    emojset["\xf0\x9f\x91\x9b"] = "f09f919b";
    emojset["\xf0\x9f\x91\x9c"] = "f09f919c";
    emojset["\xf0\x9f\x91\x9d"] = "f09f919d";
    emojset["\xf0\x9f\x92\xb0"] = "f09f92b0";
    emojset["\xf0\x9f\x92\xb1"] = "f09f92b1";
    emojset["\xf0\x9f\x92\xb9"] = "f09f92b9";
    emojset["\xf0\x9f\x92\xb2"] = "f09f92b2";
    emojset["\xf0\x9f\x92\xb3"] = "f09f92b3";
    emojset["\xf0\x9f\x92\xb4"] = "f09f92b4";
    emojset["\xf0\x9f\x92\xb5"] = "f09f92b5";
    emojset["\xf0\x9f\x92\xb8"] = "f09f92b8";
    emojset["\xf0\x9f\x87\xa8\xf0\x9f\x87\xb3"] = "f09f87a8f09f87b3";
    emojset["\xf0\x9f\x87\xa9\xf0\x9f\x87\xaa"] = "f09f87a9f09f87aa";
    emojset["\xf0\x9f\x87\xaa\xf0\x9f\x87\xb8"] = "f09f87aaf09f87b8";
    emojset["\xf0\x9f\x87\xab\xf0\x9f\x87\xb7"] = "f09f87abf09f87b7";
    emojset["\xf0\x9f\x87\xac\xf0\x9f\x87\xa7"] = "f09f87acf09f87a7";
    emojset["\xf0\x9f\x87\xae\xf0\x9f\x87\xb9"] = "f09f87aef09f87b9";
    emojset["\xf0\x9f\x87\xaf\xf0\x9f\x87\xb5"] = "f09f87aff09f87b5";
    emojset["\xf0\x9f\x87\xb0\xf0\x9f\x87\xb7"] = "f09f87b0f09f87b7";
    emojset["\xf0\x9f\x87\xb7\xf0\x9f\x87\xba"] = "f09f87b7f09f87ba";
    emojset["\xf0\x9f\x87\xba\xf0\x9f\x87\xb8"] = "f09f87baf09f87b8";
    emojset["\xf0\x9f\x94\xa5"] = "f09f94a5";
    emojset["\xf0\x9f\x94\xa6"] = "f09f94a6";
    emojset["\xf0\x9f\x94\xa7"] = "f09f94a7";
    emojset["\xf0\x9f\x94\xa8"] = "f09f94a8";
    emojset["\xf0\x9f\x94\xa9"] = "f09f94a9";
    emojset["\xf0\x9f\x94\xaa"] = "f09f94aa";
    emojset["\xf0\x9f\x94\xab"] = "f09f94ab";
    emojset["\xf0\x9f\x94\xae"] = "f09f94ae";
    emojset["\xf0\x9f\x94\xaf"] = "f09f94af";
    emojset["\xf0\x9f\x94\xb0"] = "f09f94b0";
    emojset["\xf0\x9f\x94\xb1"] = "f09f94b1";
    emojset["\xf0\x9f\x92\x89"] = "f09f9289";
    emojset["\xf0\x9f\x92\x8a"] = "f09f928a";
    emojset["\xf0\x9f\x85\xb0"] = "f09f85b0";
    emojset["\xf0\x9f\x85\xb1"] = "f09f85b1";
    emojset["\xf0\x9f\x86\x8e"] = "f09f868e";
    emojset["\xf0\x9f\x85\xbe"] = "f09f85be";
    emojset["\xf0\x9f\x8e\x80"] = "f09f8e80";
    emojset["\xf0\x9f\x8e\x81"] = "f09f8e81";
    emojset["\xf0\x9f\x8e\x82"] = "f09f8e82";
    emojset["\xf0\x9f\x8e\x84"] = "f09f8e84";
    emojset["\xf0\x9f\x8e\x85"] = "f09f8e85";
    emojset["\xf0\x9f\x8e\x8c"] = "f09f8e8c";
    emojset["\xf0\x9f\x8e\x86"] = "f09f8e86";
    emojset["\xf0\x9f\x8e\x88"] = "f09f8e88";
    emojset["\xf0\x9f\x8e\x89"] = "f09f8e89";
    emojset["\xf0\x9f\x8e\x8d"] = "f09f8e8d";
    emojset["\xf0\x9f\x8e\x8e"] = "f09f8e8e";
    emojset["\xf0\x9f\x8e\x93"] = "f09f8e93";
    emojset["\xf0\x9f\x8e\x92"] = "f09f8e92";
    emojset["\xf0\x9f\x8e\x8f"] = "f09f8e8f";
    emojset["\xf0\x9f\x8e\x87"] = "f09f8e87";
    emojset["\xf0\x9f\x8e\x90"] = "f09f8e90";
    emojset["\xf0\x9f\x8e\x83"] = "f09f8e83";
    emojset["\xf0\x9f\x8e\x8a"] = "f09f8e8a";
    emojset["\xf0\x9f\x8e\x8b"] = "f09f8e8b";
    emojset["\xf0\x9f\x8e\x91"] = "f09f8e91";
    emojset["\xf0\x9f\x93\x9f"] = "f09f939f";
    emojset["\xe2\x98\x8e"] = "e2988e";
    emojset["\xf0\x9f\x93\x9e"] = "f09f939e";
    emojset["\xf0\x9f\x93\xb1"] = "f09f93b1";
    emojset["\xf0\x9f\x93\xb2"] = "f09f93b2";
    emojset["\xf0\x9f\x93\x9d"] = "f09f939d";
    emojset["\xf0\x9f\x93\xa0"] = "f09f93a0";
    emojset["\xe2\x9c\x89"] = "e29c89";
    emojset["\xf0\x9f\x93\xa8"] = "f09f93a8";
    emojset["\xf0\x9f\x93\xa9"] = "f09f93a9";
    emojset["\xf0\x9f\x93\xaa"] = "f09f93aa";
    emojset["\xf0\x9f\x93\xab"] = "f09f93ab";
    emojset["\xf0\x9f\x93\xae"] = "f09f93ae";
    emojset["\xf0\x9f\x93\xb0"] = "f09f93b0";
    emojset["\xf0\x9f\x93\xa2"] = "f09f93a2";
    emojset["\xf0\x9f\x93\xa3"] = "f09f93a3";
    emojset["\xf0\x9f\x93\xa1"] = "f09f93a1";
    emojset["\xf0\x9f\x93\xa4"] = "f09f93a4";
    emojset["\xf0\x9f\x93\xa5"] = "f09f93a5";
    emojset["\xf0\x9f\x93\xa6"] = "f09f93a6";
    emojset["\xf0\x9f\x93\xa7"] = "f09f93a7";
    emojset["\xf0\x9f\x94\xa0"] = "f09f94a0";
    emojset["\xf0\x9f\x94\xa1"] = "f09f94a1";
    emojset["\xf0\x9f\x94\xa2"] = "f09f94a2";
    emojset["\xf0\x9f\x94\xa3"] = "f09f94a3";
    emojset["\xf0\x9f\x94\xa4"] = "f09f94a4";
    emojset["\xe2\x9c\x92"] = "e29c92";
    emojset["\xf0\x9f\x92\xba"] = "f09f92ba";
    emojset["\xf0\x9f\x92\xbb"] = "f09f92bb";
    emojset["\xe2\x9c\x8f"] = "e29c8f";
    emojset["\xf0\x9f\x93\x8e"] = "f09f938e";
    emojset["\xf0\x9f\x92\xbc"] = "f09f92bc";
    emojset["\xf0\x9f\x92\xbd"] = "f09f92bd";
    emojset["\xf0\x9f\x92\xbe"] = "f09f92be";
    emojset["\xf0\x9f\x92\xbf"] = "f09f92bf";
    emojset["\xf0\x9f\x93\x80"] = "f09f9380";
    emojset["\xe2\x9c\x82"] = "e29c82";
    emojset["\xf0\x9f\x93\x8d"] = "f09f938d";
    emojset["\xf0\x9f\x93\x83"] = "f09f9383";
    emojset["\xf0\x9f\x93\x84"] = "f09f9384";
    emojset["\xf0\x9f\x93\x85"] = "f09f9385";
    emojset["\xf0\x9f\x93\x81"] = "f09f9381";
    emojset["\xf0\x9f\x93\x82"] = "f09f9382";
    emojset["\xf0\x9f\x93\x93"] = "f09f9393";
    emojset["\xf0\x9f\x93\x96"] = "f09f9396";
    emojset["\xf0\x9f\x93\x94"] = "f09f9394";
    emojset["\xf0\x9f\x93\x95"] = "f09f9395";
    emojset["\xf0\x9f\x93\x97"] = "f09f9397";
    emojset["\xf0\x9f\x93\x98"] = "f09f9398";
    emojset["\xf0\x9f\x93\x99"] = "f09f9399";
    emojset["\xf0\x9f\x93\x9a"] = "f09f939a";
    emojset["\xf0\x9f\x93\x9b"] = "f09f939b";
    emojset["\xf0\x9f\x93\x9c"] = "f09f939c";
    emojset["\xf0\x9f\x93\x8b"] = "f09f938b";
    emojset["\xf0\x9f\x93\x86"] = "f09f9386";
    emojset["\xf0\x9f\x93\x8a"] = "f09f938a";
    emojset["\xf0\x9f\x93\x88"] = "f09f9388";
    emojset["\xf0\x9f\x93\x89"] = "f09f9389";
    emojset["\xf0\x9f\x93\x87"] = "f09f9387";
    emojset["\xf0\x9f\x93\x8c"] = "f09f938c";
    emojset["\xf0\x9f\x93\x92"] = "f09f9392";
    emojset["\xf0\x9f\x93\x8f"] = "f09f938f";
    emojset["\xf0\x9f\x93\x90"] = "f09f9390";
    emojset["\xf0\x9f\x93\x91"] = "f09f9391";
    emojset["\xf0\x9f\x8e\xbd"] = "f09f8ebd";
    emojset["\xe2\x9a\xbe"] = "e29abe";
    emojset["\xe2\x9b\xb3"] = "e29bb3";
    emojset["\xf0\x9f\x8e\xbe"] = "f09f8ebe";
    emojset["\xe2\x9a\xbd"] = "e29abd";
    emojset["\xf0\x9f\x8e\xbf"] = "f09f8ebf";
    emojset["\xf0\x9f\x8f\x80"] = "f09f8f80";
    emojset["\xf0\x9f\x8f\x81"] = "f09f8f81";
    emojset["\xf0\x9f\x8f\x82"] = "f09f8f82";
    emojset["\xf0\x9f\x8f\x83"] = "f09f8f83";
    emojset["\xf0\x9f\x8f\x84"] = "f09f8f84";
    emojset["\xf0\x9f\x8f\x86"] = "f09f8f86";
    emojset["\xf0\x9f\x8f\x88"] = "f09f8f88";
    emojset["\xf0\x9f\x8f\x8a"] = "f09f8f8a";
    emojset["\xf0\x9f\x9a\x83"] = "f09f9a83";
    emojset["\xf0\x9f\x9a\x87"] = "f09f9a87";
    emojset["\xe2\x93\x82"] = "e29382";
    emojset["\xf0\x9f\x9a\x84"] = "f09f9a84";
    emojset["\xf0\x9f\x9a\x85"] = "f09f9a85";
    emojset["\xf0\x9f\x9a\x97"] = "f09f9a97";
    emojset["\xf0\x9f\x9a\x99"] = "f09f9a99";
    emojset["\xf0\x9f\x9a\x8c"] = "f09f9a8c";
    emojset["\xf0\x9f\x9a\x8f"] = "f09f9a8f";
    emojset["\xf0\x9f\x9a\xa2"] = "f09f9aa2";
    emojset["\xe2\x9c\x88"] = "e29c88";
    emojset["\xe2\x9b\xb5"] = "e29bb5";
    emojset["\xf0\x9f\x9a\x89"] = "f09f9a89";
    emojset["\xf0\x9f\x9a\x80"] = "f09f9a80";
    emojset["\xf0\x9f\x9a\xa4"] = "f09f9aa4";
    emojset["\xf0\x9f\x9a\x95"] = "f09f9a95";
    emojset["\xf0\x9f\x9a\x9a"] = "f09f9a9a";
    emojset["\xf0\x9f\x9a\x92"] = "f09f9a92";
    emojset["\xf0\x9f\x9a\x91"] = "f09f9a91";
    emojset["\xf0\x9f\x9a\x93"] = "f09f9a93";
    emojset["\xe2\x9b\xbd"] = "e29bbd";
    emojset["\xf0\x9f\x85\xbf"] = "f09f85bf";
    emojset["\xf0\x9f\x9a\xa5"] = "f09f9aa5";
    emojset["\xf0\x9f\x9a\xa7"] = "f09f9aa7";
    emojset["\xf0\x9f\x9a\xa8"] = "f09f9aa8";
    emojset["\xe2\x99\xa8"] = "e299a8";
    emojset["\xe2\x9b\xba"] = "e29bba";
    emojset["\xf0\x9f\x8e\xa0"] = "f09f8ea0";
    emojset["\xf0\x9f\x8e\xa1"] = "f09f8ea1";
    emojset["\xf0\x9f\x8e\xa2"] = "f09f8ea2";
    emojset["\xf0\x9f\x8e\xa3"] = "f09f8ea3";
    emojset["\xf0\x9f\x8e\xa4"] = "f09f8ea4";
    emojset["\xf0\x9f\x8e\xa5"] = "f09f8ea5";
    emojset["\xf0\x9f\x8e\xa6"] = "f09f8ea6";
    emojset["\xf0\x9f\x8e\xa7"] = "f09f8ea7";
    emojset["\xf0\x9f\x8e\xa8"] = "f09f8ea8";
    emojset["\xf0\x9f\x8e\xa9"] = "f09f8ea9";
    emojset["\xf0\x9f\x8e\xaa"] = "f09f8eaa";
    emojset["\xf0\x9f\x8e\xab"] = "f09f8eab";
    emojset["\xf0\x9f\x8e\xac"] = "f09f8eac";
    emojset["\xf0\x9f\x8e\xad"] = "f09f8ead";
    emojset["\xf0\x9f\x8e\xae"] = "f09f8eae";
    emojset["\xf0\x9f\x80\x84"] = "f09f8084";
    emojset["\xf0\x9f\x8e\xaf"] = "f09f8eaf";
    emojset["\xf0\x9f\x8e\xb0"] = "f09f8eb0";
    emojset["\xf0\x9f\x8e\xb1"] = "f09f8eb1";
    emojset["\xf0\x9f\x8e\xb2"] = "f09f8eb2";
    emojset["\xf0\x9f\x8e\xb3"] = "f09f8eb3";
    emojset["\xf0\x9f\x8e\xb4"] = "f09f8eb4";
    emojset["\xf0\x9f\x83\x8f"] = "f09f838f";
    emojset["\xf0\x9f\x8e\xb5"] = "f09f8eb5";
    emojset["\xf0\x9f\x8e\xb6"] = "f09f8eb6";
    emojset["\xf0\x9f\x8e\xb7"] = "f09f8eb7";
    emojset["\xf0\x9f\x8e\xb8"] = "f09f8eb8";
    emojset["\xf0\x9f\x8e\xb9"] = "f09f8eb9";
    emojset["\xf0\x9f\x8e\xba"] = "f09f8eba";
    emojset["\xf0\x9f\x8e\xbb"] = "f09f8ebb";
    emojset["\xf0\x9f\x8e\xbc"] = "f09f8ebc";
    emojset["\xe3\x80\xbd"] = "e380bd";
    emojset["\xf0\x9f\x93\xb7"] = "f09f93b7";
    emojset["\xf0\x9f\x93\xb9"] = "f09f93b9";
    emojset["\xf0\x9f\x93\xba"] = "f09f93ba";
    emojset["\xf0\x9f\x93\xbb"] = "f09f93bb";
    emojset["\xf0\x9f\x93\xbc"] = "f09f93bc";
    emojset["\xf0\x9f\x92\x8b"] = "f09f928b";
    emojset["\xf0\x9f\x92\x8c"] = "f09f928c";
    emojset["\xf0\x9f\x92\x8d"] = "f09f928d";
    emojset["\xf0\x9f\x92\x8e"] = "f09f928e";
    emojset["\xf0\x9f\x92\x8f"] = "f09f928f";
    emojset["\xf0\x9f\x92\x90"] = "f09f9290";
    emojset["\xf0\x9f\x92\x91"] = "f09f9291";
    emojset["\xf0\x9f\x92\x92"] = "f09f9292";
    emojset["\xf0\x9f\x94\x9e"] = "f09f949e";
    emojset["\xc2\xa9"] = "c2a9";
    emojset["\xc2\xae"] = "c2ae";
    emojset["\xe2\x84\xa2"] = "e284a2";
    emojset["\xe2\x84\xb9"] = "e284b9";
    emojset["#\xe2\x83\xa3"] = "#e283a3";
    emojset["1\xe2\x83\xa3"] = "1e283a3";
    emojset["2\xe2\x83\xa3"] = "2e283a3";
    emojset["3\xe2\x83\xa3"] = "3e283a3";
    emojset["4\xe2\x83\xa3"] = "4e283a3";
    emojset["5\xe2\x83\xa3"] = "5e283a3";
    emojset["6\xe2\x83\xa3"] = "6e283a3";
    emojset["7\xe2\x83\xa3"] = "7e283a3";
    emojset["8\xe2\x83\xa3"] = "8e283a3";
    emojset["9\xe2\x83\xa3"] = "9e283a3";
    emojset["0\xe2\x83\xa3"] = "0e283a3";
    emojset["\xf0\x9f\x94\x9f"] = "f09f949f";
    emojset["\xf0\x9f\x93\xb6"] = "f09f93b6";
    emojset["\xf0\x9f\x93\xb3"] = "f09f93b3";
    emojset["\xf0\x9f\x93\xb4"] = "f09f93b4";
    emojset["\xf0\x9f\x8d\x94"] = "f09f8d94";
    emojset["\xf0\x9f\x8d\x99"] = "f09f8d99";
    emojset["\xf0\x9f\x8d\xb0"] = "f09f8db0";
    emojset["\xf0\x9f\x8d\x9c"] = "f09f8d9c";
    emojset["\xf0\x9f\x8d\x9e"] = "f09f8d9e";
    emojset["\xf0\x9f\x8d\xb3"] = "f09f8db3";
    emojset["\xf0\x9f\x8d\xa6"] = "f09f8da6";
    emojset["\xf0\x9f\x8d\x9f"] = "f09f8d9f";
    emojset["\xf0\x9f\x8d\xa1"] = "f09f8da1";
    emojset["\xf0\x9f\x8d\x98"] = "f09f8d98";
    emojset["\xf0\x9f\x8d\x9a"] = "f09f8d9a";
    emojset["\xf0\x9f\x8d\x9d"] = "f09f8d9d";
    emojset["\xf0\x9f\x8d\x9b"] = "f09f8d9b";
    emojset["\xf0\x9f\x8d\xa2"] = "f09f8da2";
    emojset["\xf0\x9f\x8d\xa3"] = "f09f8da3";
    emojset["\xf0\x9f\x8d\xb1"] = "f09f8db1";
    emojset["\xf0\x9f\x8d\xb2"] = "f09f8db2";
    emojset["\xf0\x9f\x8d\xa7"] = "f09f8da7";
    emojset["\xf0\x9f\x8d\x96"] = "f09f8d96";
    emojset["\xf0\x9f\x8d\xa5"] = "f09f8da5";
    emojset["\xf0\x9f\x8d\xa0"] = "f09f8da0";
    emojset["\xf0\x9f\x8d\x95"] = "f09f8d95";
    emojset["\xf0\x9f\x8d\x97"] = "f09f8d97";
    emojset["\xf0\x9f\x8d\xa8"] = "f09f8da8";
    emojset["\xf0\x9f\x8d\xa9"] = "f09f8da9";
    emojset["\xf0\x9f\x8d\xaa"] = "f09f8daa";
    emojset["\xf0\x9f\x8d\xab"] = "f09f8dab";
    emojset["\xf0\x9f\x8d\xac"] = "f09f8dac";
    emojset["\xf0\x9f\x8d\xad"] = "f09f8dad";
    emojset["\xf0\x9f\x8d\xae"] = "f09f8dae";
    emojset["\xf0\x9f\x8d\xaf"] = "f09f8daf";
    emojset["\xf0\x9f\x8d\xa4"] = "f09f8da4";
    emojset["\xf0\x9f\x8d\xb4"] = "f09f8db4";
    emojset["\xe2\x98\x95"] = "e29895";
    emojset["\xf0\x9f\x8d\xb8"] = "f09f8db8";
    emojset["\xf0\x9f\x8d\xba"] = "f09f8dba";
    emojset["\xf0\x9f\x8d\xb5"] = "f09f8db5";
    emojset["\xf0\x9f\x8d\xb6"] = "f09f8db6";
    emojset["\xf0\x9f\x8d\xb7"] = "f09f8db7";
    emojset["\xf0\x9f\x8d\xbb"] = "f09f8dbb";
    emojset["\xf0\x9f\x8d\xb9"] = "f09f8db9";
    emojset["\xe2\x86\x97"] = "e28697";
    emojset["\xe2\x86\x98"] = "e28698";
    emojset["\xe2\x86\x96"] = "e28696";
    emojset["\xe2\x86\x99"] = "e28699";
    emojset["\xe2\xa4\xb4"] = "e2a4b4";
    emojset["\xe2\xa4\xb5"] = "e2a4b5";
    emojset["\xe2\x86\x94"] = "e28694";
    emojset["\xe2\x86\x95"] = "e28695";
    emojset["\xe2\xac\x86"] = "e2ac86";
    emojset["\xe2\xac\x87"] = "e2ac87";
    emojset["\xe2\x9e\xa1"] = "e29ea1";
    emojset["\xe2\xac\x85"] = "e2ac85";
    emojset["\xe2\x96\xb6"] = "e296b6";
    emojset["\xe2\x97\x80"] = "e29780";
    emojset["\xe2\x8f\xa9"] = "e28fa9";
    emojset["\xe2\x8f\xaa"] = "e28faa";
    emojset["\xe2\x8f\xab"] = "e28fab";
    emojset["\xe2\x8f\xac"] = "e28fac";
    emojset["\xf0\x9f\x94\xba"] = "f09f94ba";
    emojset["\xf0\x9f\x94\xbb"] = "f09f94bb";
    emojset["\xf0\x9f\x94\xbc"] = "f09f94bc";
    emojset["\xf0\x9f\x94\xbd"] = "f09f94bd";
    emojset["\xe2\xad\x95"] = "e2ad95";
    emojset["\xe2\x9d\x8c"] = "e29d8c";
    emojset["\xe2\x9d\x8e"] = "e29d8e";
    emojset["\xe2\x9d\x97"] = "e29d97";
    emojset["\xe2\x81\x89"] = "e28189";
    emojset["\xe2\x80\xbc"] = "e280bc";
    emojset["\xe2\x9d\x93"] = "e29d93";
    emojset["\xe2\x9d\x94"] = "e29d94";
    emojset["\xe2\x9d\x95"] = "e29d95";
    emojset["\xe3\x80\xb0"] = "e380b0";
    emojset["\xe2\x9e\xb0"] = "e29eb0";
    emojset["\xe2\x9e\xbf"] = "e29ebf";
    emojset["\xe2\x9d\xa4"] = "e29da4";
    emojset["\xf0\x9f\x92\x93"] = "f09f9293";
    emojset["\xf0\x9f\x92\x94"] = "f09f9294";
    emojset["\xf0\x9f\x92\x95"] = "f09f9295";
    emojset["\xf0\x9f\x92\x96"] = "f09f9296";
    emojset["\xf0\x9f\x92\x97"] = "f09f9297";
    emojset["\xf0\x9f\x92\x98"] = "f09f9298";
    emojset["\xf0\x9f\x92\x99"] = "f09f9299";
    emojset["\xf0\x9f\x92\x9a"] = "f09f929a";
    emojset["\xf0\x9f\x92\x9b"] = "f09f929b";
    emojset["\xf0\x9f\x92\x9c"] = "f09f929c";
    emojset["\xf0\x9f\x92\x9d"] = "f09f929d";
    emojset["\xf0\x9f\x92\x9e"] = "f09f929e";
    emojset["\xf0\x9f\x92\x9f"] = "f09f929f";
    emojset["\xe2\x99\xa5"] = "e299a5";
    emojset["\xe2\x99\xa0"] = "e299a0";
    emojset["\xe2\x99\xa6"] = "e299a6";
    emojset["\xe2\x99\xa3"] = "e299a3";
    emojset["\xf0\x9f\x9a\xac"] = "f09f9aac";
    emojset["\xf0\x9f\x9a\xad"] = "f09f9aad";
    emojset["\xe2\x99\xbf"] = "e299bf";
    emojset["\xf0\x9f\x9a\xa9"] = "f09f9aa9";
    emojset["\xe2\x9a\xa0"] = "e29aa0";
    emojset["\xe2\x9b\x94"] = "e29b94";
    emojset["\xe2\x99\xbb"] = "e299bb";
    emojset["\xf0\x9f\x9a\xb2"] = "f09f9ab2";
    emojset["\xf0\x9f\x9a\xb6"] = "f09f9ab6";
    emojset["\xf0\x9f\x9a\xb9"] = "f09f9ab9";
    emojset["\xf0\x9f\x9a\xba"] = "f09f9aba";
    emojset["\xf0\x9f\x9b\x80"] = "f09f9b80";
    emojset["\xf0\x9f\x9a\xbb"] = "f09f9abb";
    emojset["\xf0\x9f\x9a\xbd"] = "f09f9abd";
    emojset["\xf0\x9f\x9a\xbe"] = "f09f9abe";
    emojset["\xf0\x9f\x9a\xbc"] = "f09f9abc";
    emojset["\xf0\x9f\x9a\xaa"] = "f09f9aaa";
    emojset["\xf0\x9f\x9a\xab"] = "f09f9aab";
    emojset["\xe2\x9c\x94"] = "e29c94";
    emojset["\xf0\x9f\x86\x91"] = "f09f8691";
    emojset["\xf0\x9f\x86\x92"] = "f09f8692";
    emojset["\xf0\x9f\x86\x93"] = "f09f8693";
    emojset["\xf0\x9f\x86\x94"] = "f09f8694";
    emojset["\xf0\x9f\x86\x95"] = "f09f8695";
    emojset["\xf0\x9f\x86\x96"] = "f09f8696";
    emojset["\xf0\x9f\x86\x97"] = "f09f8697";
    emojset["\xf0\x9f\x86\x98"] = "f09f8698";
    emojset["\xf0\x9f\x86\x99"] = "f09f8699";
    emojset["\xf0\x9f\x86\x9a"] = "f09f869a";
    emojset["\xf0\x9f\x88\x81"] = "f09f8881";
    emojset["\xf0\x9f\x88\x82"] = "f09f8882";
    emojset["\xf0\x9f\x88\xb2"] = "f09f88b2";
    emojset["\xf0\x9f\x88\xb3"] = "f09f88b3";
    emojset["\xf0\x9f\x88\xb4"] = "f09f88b4";
    emojset["\xf0\x9f\x88\xb5"] = "f09f88b5";
    emojset["\xf0\x9f\x88\xb6"] = "f09f88b6";
    emojset["\xf0\x9f\x88\x9a"] = "f09f889a";
    emojset["\xf0\x9f\x88\xb7"] = "f09f88b7";
    emojset["\xf0\x9f\x88\xb8"] = "f09f88b8";
    emojset["\xf0\x9f\x88\xb9"] = "f09f88b9";
    emojset["\xf0\x9f\x88\xaf"] = "f09f88af";
    emojset["\xf0\x9f\x88\xba"] = "f09f88ba";
    emojset["\xe3\x8a\x99"] = "e38a99";
    emojset["\xe3\x8a\x97"] = "e38a97";
    emojset["\xf0\x9f\x89\x90"] = "f09f8990";
    emojset["\xf0\x9f\x89\x91"] = "f09f8991";
    emojset["\xe2\x9e\x95"] = "e29e95";
    emojset["\xe2\x9e\x96"] = "e29e96";
    emojset["\xe2\x9c\x96"] = "e29c96";
    emojset["\xe2\x9e\x97"] = "e29e97";
    emojset["\xf0\x9f\x92\xa0"] = "f09f92a0";
    emojset["\xf0\x9f\x92\xa1"] = "f09f92a1";
    emojset["\xf0\x9f\x92\xa2"] = "f09f92a2";
    emojset["\xf0\x9f\x92\xa3"] = "f09f92a3";
    emojset["\xf0\x9f\x92\xa4"] = "f09f92a4";
    emojset["\xf0\x9f\x92\xa5"] = "f09f92a5";
    emojset["\xf0\x9f\x92\xa6"] = "f09f92a6";
    emojset["\xf0\x9f\x92\xa7"] = "f09f92a7";
    emojset["\xf0\x9f\x92\xa8"] = "f09f92a8";
    emojset["\xf0\x9f\x92\xa9"] = "f09f92a9";
    emojset["\xf0\x9f\x92\xaa"] = "f09f92aa";
    emojset["\xf0\x9f\x92\xab"] = "f09f92ab";
    emojset["\xf0\x9f\x92\xac"] = "f09f92ac";
    emojset["\xe2\x9c\xa8"] = "e29ca8";
    emojset["\xe2\x9c\xb4"] = "e29cb4";
    emojset["\xe2\x9c\xb3"] = "e29cb3";
    emojset["\xe2\x9a\xaa"] = "e29aaa";
    emojset["\xe2\x9a\xab"] = "e29aab";
    emojset["\xf0\x9f\x94\xb4"] = "f09f94b4";
    emojset["\xf0\x9f\x94\xb5"] = "f09f94b5";
    emojset["\xf0\x9f\x94\xb2"] = "f09f94b2";
    emojset["\xf0\x9f\x94\xb3"] = "f09f94b3";
    emojset["\xe2\xad\x90"] = "e2ad90";
    emojset["\xe2\xac\x9c"] = "e2ac9c";
    emojset["\xe2\xac\x9b"] = "e2ac9b";
    emojset["\xe2\x96\xab"] = "e296ab";
    emojset["\xe2\x96\xaa"] = "e296aa";
    emojset["\xe2\x97\xbd"] = "e297bd";
    emojset["\xe2\x97\xbe"] = "e297be";
    emojset["\xe2\x97\xbb"] = "e297bb";
    emojset["\xe2\x97\xbc"] = "e297bc";
    emojset["\xf0\x9f\x94\xb6"] = "f09f94b6";
    emojset["\xf0\x9f\x94\xb7"] = "f09f94b7";
    emojset["\xf0\x9f\x94\xb8"] = "f09f94b8";
    emojset["\xf0\x9f\x94\xb9"] = "f09f94b9";
    emojset["\xe2\x9d\x87"] = "e29d87";
    emojset["\xf0\x9f\x92\xae"] = "f09f92ae";
    emojset["\xf0\x9f\x92\xaf"] = "f09f92af";
    emojset["\xe2\x86\xa9"] = "e286a9";
    emojset["\xe2\x86\xaa"] = "e286aa";
    emojset["\xf0\x9f\x94\x83"] = "f09f9483";
    emojset["\xf0\x9f\x94\x8a"] = "f09f948a";
    emojset["\xf0\x9f\x94\x8b"] = "f09f948b";
    emojset["\xf0\x9f\x94\x8c"] = "f09f948c";
    emojset["\xf0\x9f\x94\x8d"] = "f09f948d";
    emojset["\xf0\x9f\x94\x8e"] = "f09f948e";
    emojset["\xf0\x9f\x94\x92"] = "f09f9492";
    emojset["\xf0\x9f\x94\x93"] = "f09f9493";
    emojset["\xf0\x9f\x94\x8f"] = "f09f948f";
    emojset["\xf0\x9f\x94\x90"] = "f09f9490";
    emojset["\xf0\x9f\x94\x91"] = "f09f9491";
    emojset["\xf0\x9f\x94\x94"] = "f09f9494";
    emojset["\xe2\x98\x91"] = "e29891";
    emojset["\xf0\x9f\x94\x98"] = "f09f9498";
    emojset["\xf0\x9f\x94\x96"] = "f09f9496";
    emojset["\xf0\x9f\x94\x97"] = "f09f9497";
    emojset["\xf0\x9f\x94\x99"] = "f09f9499";
    emojset["\xf0\x9f\x94\x9a"] = "f09f949a";
    emojset["\xf0\x9f\x94\x9b"] = "f09f949b";
    emojset["\xf0\x9f\x94\x9c"] = "f09f949c";
    emojset["\xf0\x9f\x94\x9d"] = "f09f949d";
    emojset["\xe2\x80\x83"] = "e28083";
    emojset["\xe2\x80\x82"] = "e28082";
    emojset["\xe2\x80\x85"] = "e28085";
    emojset["\xe2\x9c\x85"] = "e29c85";
    emojset["\xe2\x9c\x8a"] = "e29c8a";
    emojset["\xe2\x9c\x8b"] = "e29c8b";
    emojset["\xe2\x9c\x8c"] = "e29c8c";
    emojset["\xf0\x9f\x91\x8a"] = "f09f918a";
    emojset["\xf0\x9f\x91\x8d"] = "f09f918d";
    emojset["\xe2\x98\x9d"] = "e2989d";
    emojset["\xf0\x9f\x91\x86"] = "f09f9186";
    emojset["\xf0\x9f\x91\x87"] = "f09f9187";
    emojset["\xf0\x9f\x91\x88"] = "f09f9188";
    emojset["\xf0\x9f\x91\x89"] = "f09f9189";
    emojset["\xf0\x9f\x91\x8b"] = "f09f918b";
    emojset["\xf0\x9f\x91\x8f"] = "f09f918f";
    emojset["\xf0\x9f\x91\x8c"] = "f09f918c";
    emojset["\xf0\x9f\x91\x8e"] = "f09f918e";
    emojset["\xf0\x9f\x91\x90"] = "f09f9190";
    return;
}

void CEmojConvert::convert(const string& from, string& to)
{
    setlocale(LC_ALL, "en_US.utf8");
    const char* pt = from.c_str();
    int max = from.length();
    //std::cout << "from [" << from << "] max [" << max << "]" << std::endl;
    //std::cout << "to [" << to << "]" << std::endl;
    while (max>0) 
    {
        string word;
        
        unsigned length = mblen(pt, max);
        if (length<1) break;
        
        word = string(pt, length);
        //std::cout << "word [" << word << "], length [" << length << "]" << std::endl;

        bool emoj = false;
        map<string, string>::iterator it = emojset.begin();
        for(;it!=emojset.end();++it)
        {
            //unsigned length = mblen(it->first.c_str(), 100);
            //std::cout << it->first << " : " << it ->second << "length [" << length << "]" << std::endl;
            if(word == it->first)
            {
                emoj = true;
                break;
            }
        }
        
        if(emoj)
            to += "[emoji:" + it->second + "]";
        else
            to += word;

        pt+=length; max-=length;
    }
}

/*
int main(int argc, char* argv[])
{   
    CEmojConvert::init();

    //string from(argv[1]);
    string from("\xf0\x9f\x91\x8e\xf0\x9f\x91\x90\xf0\x9f\x91\x90\xf0\x9f\x91\x90\xf0\x9f\x91\x90zddf周松泉\xf0\x9f\x91\x8e");
    string to;
    CEmojConvert::convert(from, to);
    std::cout << "from [" << from << "] to [" << to << "]" << std::endl;
    return 0;
}
*/
