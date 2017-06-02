#ifndef  _REDIS_CLIENTS_H
#define  _REDIS_CLIENTS_H

#include <vector>
#include <utility>
#include <string.h>
#include <stdint.h>
#include <sstream>
#include <iostream>
#include <string>
#include <stdlib.h>
#include "hiredis.h"

class redis_str{
public:
   redis_str(){
       point = NULL;
       len = 0;
       lens =NULL;
   }
   ~redis_str(){
       if(point){
           for(unsigned int i = 0; i < len; i++){
              if(point[i])
                  free(point[i]);
           }
           free(point);
       }
       if(lens) free(lens); 
   }
   int redis_malloc(unsigned int s){
       if(point){
          for(unsigned int i = 0; i < len; i++){
              if(point[i])
                  free(point[i]);
          }
          free(point);
          point = NULL;
       }
       if(lens){
          free(lens);
          lens = NULL;
       }
       point = (char**)malloc(s*sizeof(char*));
       lens  = (size_t*)malloc(s*sizeof(size_t));
       if( NULL== point || NULL == lens )return -1;
       len = s;
       for(unsigned int i = 0; i < len; i++){
          point[i]=NULL;
          lens[i] = 0;
       }
       return s;
   };
   int redis_add(unsigned int index,const char* s, unsigned int l){
       if(index >= len || NULL == point ) return -1;
       if(point[index]){
           free(point[index]);
           point[index] = NULL;
       }
       point[index]= (char*)malloc(l+1);
       if(NULL == point[index])return -1;
       memset(point[index],0,l+1);
       memcpy(point[index],s,l);
       lens[index] = l;
       return l;
   }
   char**          point;
   unsigned int    len;
   size_t*         lens;
};

class redis_clients
{
public:
    redis_clients(int dbindex);
    ~redis_clients();
    int add_host(const std::string, int);
    int set(const std::string&, const std::string&);
    int setx( const std::string& , const std::string& , int  );
    int get(const std::string&, std::string*);
    int multi_get(const std::vector<std::string> &, std::vector<std::string> * );
    int multi_exists(const std::vector<std::string> &, std::vector<std::string> * );
    int expire(const std::string &, int64_t );
    int incr(const std::string&, int64_t, int64_t*);
    int del(const std::string&);
    int hset(const std::string&, const std::string&, const std::string &);
    int hget(const std::string&, const std::string&, std::string *val);
    int multi_hget(const std::string &, const std::vector<std::string> &, std::vector<std::string> * );
    int multi_hexists(const std::string &, const std::vector<std::string> &, std::vector<std::string> * );
    int hdel(const std::string&, const std::string&);
    int hkeys(const std::string &, const std::string&, const std::string&, uint64_t, std::vector<std::string> *);
    int hkeys(const std::string &, std::vector<std::string> *);
    int hscan(const std::string &, const std::string&, const std::string&, uint64_t, std::vector<std::string> *);
    int hscan(const std::string &, std::vector<std::string> *);
    int hincr(const std::string &, const std::string&, int64_t, int64_t *);
    int hclear(const std::string& );
    int zset(const std::string&, const std::string&, int64_t);
    int zget(const std::string&, const std::string&, int64_t*);
    int zdel(const std::string&, const std::string&);
    int zclear(const std::string&);
    int zkeys(const std::string &, const std::string&, int64_t*, int64_t*,  uint64_t, std::vector<std::string> *);
    int zkeys(const std::string &, std::vector<std::string> *);
    int zscan(const std::string &, const std::string&, int64_t*, int64_t*,  uint64_t, std::vector<std::string> *);
    int zscan(const std::string &, std::vector<std::string> *);
    int qpush(const std::string&, const std::vector<std::string> &);
    int qpushx(const std::string&, const std::vector<std::string> &);
    int qtrim(const std::string&, uint32_t , uint32_t);
    int qrange(const std::string&, uint32_t, uint32_t, std::vector<std::string>&);
    int qpush_back(const std::string&, const std::string&, int64_t *);
    int qpop_front(const std::string&, std::string &);
    int qsize(const std::string&);
    int qclear(const std::string&);
    int select(const std::string&);
    int select(int);
    void set_time_out(int t) { m_timeout = t; };
private:
    void read_array(redisReply * r, std::vector<std::string>*v);
    int exe(std::vector<std::string>&, std::vector<std::string >*);
    int execute(std::vector<std::string>&,std::vector<std::string>*);
    std::vector< std::pair<std::string, int> > m_hosts;
    int            m_timeout;
    redisContext  *m_conn;
    int            m_index;
    int            m_dbindex;
};

#endif

