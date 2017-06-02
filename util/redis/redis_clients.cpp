#include "redis_clients.h"
#include <glog/logging.h>
redis_clients::redis_clients(int index){
    m_conn=NULL;
    m_dbindex = index;
    m_timeout = 2;
}

redis_clients::~redis_clients(){
   if(m_conn) redisFree(m_conn);
}

void redis_clients::read_array(redisReply * r, std::vector<std::string>*v){
   for(int i= 0; i < r->elements; i++){
       redisReply* childReply = r->element[i];
       if( childReply->type == REDIS_REPLY_STRING){
            std::string srt(childReply->str,childReply->len);
            if(v){v->push_back(srt);}
       }
       if( childReply->type == REDIS_REPLY_INTEGER){
            int rt = childReply->integer;
            std::string srt((const char*)&rt,sizeof(int));
            if(v){v->push_back(srt);}
       }
       if( childReply->type == REDIS_REPLY_NIL){
            std::string srt;
            if(v){v->push_back(srt);}
       }
       if( childReply->type == REDIS_REPLY_ARRAY ){
           read_array(childReply,v);
       }
   }
}
/*
  -1: exe false
  >=0 :exe true
  0: result false
  1: result true
*/
int redis_clients::exe(std::vector<std::string>& clist, std::vector<std::string>* rst){
    if (!m_conn){return -99;}
    int argc = clist.size();
    redis_str  ss;
    if (ss.redis_malloc(argc)<0){
        LOG(ERROR)<<"redis_malloc false";
        return -98;
    }
    for(unsigned int i = 0; i < clist.size(); i++){
        if(ss.redis_add(i,clist[i].c_str(),clist[i].length())<0){
            LOG(ERROR)<<"redis_add false";
            return -97;
        }
    }
    redisReply *reply = (redisReply*)redisCommandArgv(m_conn,argc,(const char **)ss.point,ss.lens);
    if (!reply){
        LOG(ERROR)<<"exe command false";
        return -96;
    }
    switch(reply->type)
    {
    case REDIS_REPLY_STATUS:
        if (strcmp(reply->str,"OK") == 0) return 1;
        freeReplyObject(reply);
        return 0;
    case REDIS_REPLY_ERROR:{
        freeReplyObject(reply);
        return 0;}
    case REDIS_REPLY_INTEGER:{
        int rt = reply->integer;
        if (rst){
            std::string srt((const char*)&rt,sizeof(int));
            rst->push_back(srt);
        }
        freeReplyObject(reply);
        return rt;
        }
    case REDIS_REPLY_NIL:
        return 0;
    case REDIS_REPLY_STRING:{
        if(rst){
            std::string srt(reply->str,reply->len);
            rst->push_back(srt);
        }
        freeReplyObject(reply);
        return 1;
        }
    case REDIS_REPLY_ARRAY:{
        read_array(reply,rst);
        freeReplyObject(reply);
        return (reply->elements>0)?1:0;
        }
    default:
        LOG(ERROR)<<"REDIS_REPLY_ERROR default:"<<reply->type;
        break;
    }
    freeReplyObject(reply);
    return -1;
}

int redis_clients::execute(std::vector<std::string>& clist,std::vector<std::string>* rst){
    if (m_conn){
        int rt = exe(clist,rst);
        if (rt >= 0) {return rt;}
        redisFree(m_conn);
        m_conn = NULL;
    }
    for(unsigned int i = 0; i < m_hosts.size(); i++){
        if (!m_conn){
            std::string host = m_hosts[i].first;
            int port = m_hosts[i].second;
            struct timeval timeout = { m_timeout, 0};
            m_conn = redisConnectWithTimeout(host.c_str(), port, timeout);
            if (m_conn == NULL || m_conn->err) {
                if(m_conn) {
                    redisFree(m_conn);
                    LOG(ERROR)<<"connect "<<m_hosts[i].first<<":"<<m_hosts[i].second<<" false!";
                }
                m_conn = NULL;
                continue; // connect false
            }
            redisSetTimeout(m_conn, timeout); 
            if (m_dbindex != 0){
                std::stringstream iii;
                iii<<m_dbindex;
                if(select(iii.str()) <= 0) continue;
            }
            LOG(INFO)<<"connect "<<host<<":"<<port<<" ok!";
        }
        return exe(clist,rst);
    }
    LOG(ERROR)<<"all connect false!";
    return -1;
}

int redis_clients::add_host(const std::string h, int p){
    m_hosts.push_back(make_pair(h, p));
    return 0;
}

int redis_clients::set(const std::string& k, const std::string& v){
    std::vector<std::string> vs;
    std::string cmd = "set";
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(v);
    return execute(vs,NULL);
}

int redis_clients::get(const std::string& k, std::string* v){
    std::vector<std::string> rt;
    std::vector<std::string> vs;
    std::string cmd = "get";
    vs.push_back(cmd);
    vs.push_back(k);
    int result = execute(vs,&rt);
    if( result > 0){
        if(v)v->append(rt[0]);
    }
    return result;
}

int redis_clients::del(const std::string& k){
    std::vector<std::string> vs;
    std::string cmd = "del";
    vs.push_back(cmd);
    vs.push_back(k);
    return execute(vs,NULL);
}

int redis_clients::multi_get(const std::vector<std::string> & k, std::vector<std::string> *v ){
    std::string cmd = "mget";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    for(unsigned int i = 0; i<k.size(); i++){
        vs.push_back(k[i]);
    }
    return execute(vs,v);
}

int redis_clients::multi_exists(const std::vector<std::string> &k, std::vector<std::string> * v){
    
    return 0;
}

int redis_clients::expire(const std::string &k, int64_t t){
    std::stringstream o;
    o<<t;
    std::string cmd = "expire";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(o.str());
    return execute(vs,NULL);
}

int redis_clients::hset(const std::string& k, const std::string& n, const std::string & v){
    std::string cmd = "hset";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(n);
    vs.push_back(v);
    return execute(vs,NULL);
}

int redis_clients::hget(const std::string& k, const std::string& n,  std::string * v){
    std::string cmd = "hget";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(n);
    std::vector<std::string> rt;
    int result = execute(vs,&rt);
    if( result > 0){
        if(v)v->append(rt[0]);
    }
    return result;
}

int redis_clients::hdel(const std::string& k, const std::string& n){
    std::string cmd = "hdel";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(n);
    return execute(vs,NULL);
}

int redis_clients::hclear(const std::string& k){
    std::vector<std::string> rv;
    if( hkeys(k,&rv) < 0 ){
        return -1;
    }
    std::string cmd = "hdel";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    
    for(unsigned int i = 0; i <rv.size(); i++){
        //printf("hclear %s\n",rv[i].c_str());
        vs.push_back(rv[i]);
    }
    return execute(vs,NULL);
}

int redis_clients::hkeys(const std::string & k, std::vector<std::string> *v){
    std::string cmd = "hkeys";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    return execute(vs,v);
}

int redis_clients::hscan(const std::string &k, const std::string& c, const std::string&f, uint64_t u, std::vector<std::string> *v){
    std::string cmd = "hscan";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    std::string t1 = "0";
    std::string t2 = "MATCH";
    std::string t3 = "*";
    std::string t4 = "COUNT";
    std::stringstream o;
    o<<u;
    vs.push_back(t1);
    vs.push_back(t2);
    vs.push_back(t3);
    vs.push_back(t4);
    vs.push_back(o.str());
    std::vector<std::string> tv;
    int rst = execute(vs,&tv);
    if( rst >= 1){
       for(unsigned int i =1; i<tv.size(); i++)
          v->push_back(tv[i]); 
    }
    return rst;
}

int redis_clients::hscan(const std::string &k, std::vector<std::string> *v){
    std::string cmd = "scan";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    std::string t1 = "0";
    std::string t2 = "MATCH";
    std::string t3 = "*";
    vs.push_back(t1);
    vs.push_back(t2);
    vs.push_back(t3);
    std::vector<std::string> tv;
    int rst = execute(vs,&tv);
    if( rst >= 1){
       for(unsigned int i =1; i<tv.size(); i++)
          v->push_back(tv[i]);
    }
    return rst;
}

int redis_clients::multi_hget(const std::string & k, const std::vector<std::string> & n, std::vector<std::string> * v){
    std::string cmd = "hmget";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    for(unsigned int i = 0; i < n.size(); i++){
        vs.push_back(n[i]);
    }
    return execute(vs,v);
}

int redis_clients::multi_hexists(const std::string &k, const std::vector<std::string> &n, std::vector<std::string> * v){
    return 0;
}
int redis_clients::incr(const std::string& k, int64_t n, int64_t* v){
    return 0;
}
int redis_clients::hincr(const std::string &k, const std::string& n, int64_t nn, int64_t * v){
    return 0;
}
int redis_clients::hkeys(const std::string &k, const std::string& s1, const std::string& s2, uint64_t l, std::vector<std::string> *v){
    std::string cmd = "hkeys";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    return execute(vs,v);
}
int redis_clients::qpush(const std::string & k, const std::vector<std::string> & ls)
{
    std::string cmd = "lpush";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    for (unsigned int i = 0; i<ls.size(); i++){
        vs.push_back(ls[i]);
    }
    return execute(vs,NULL);
}
int redis_clients::qpushx(const std::string & k, const std::vector<std::string> & v)
{
    std::string cmd = "lpushx";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    for (unsigned int i=0; i<v.size(); i++){
        vs.push_back(v[i]);
    }
    return execute(vs,NULL);
}
int redis_clients::qtrim(const std::string & k, uint32_t start, uint32_t end)
{
    std::string cmd = "ltrim";
    std::stringstream s,e;
    s<<start; e<<end;
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(s.str());
    vs.push_back(e.str());
    return execute(vs,NULL);
}
int redis_clients::qrange(const std::string &k, uint32_t start, uint32_t end, std::vector<std::string> & v)
{
    std::string cmd = "lrange";
    std::stringstream s,e;
    s<<start; e<<end;
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    vs.push_back(s.str());
    vs.push_back(e.str());
    return execute(vs, &v);
}
int redis_clients::qpush_back(const std::string &k, const std::string &v, int64_t * l)
{
    return 0;
}
int redis_clients::select(const std::string &db_number){
    std::string cmd = "select";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(db_number);
    return execute(vs,NULL);
}
int redis_clients::select( int db_number){
    m_dbindex = db_number;
    std::stringstream ii;
    ii<<db_number;
    return select(ii.str());
}
int redis_clients::setx(const std::string &k, const std::string & v, int t)
{
    std::string cmd = "setex";
    std::vector<std::string> vs;
    vs.push_back(cmd);
    vs.push_back(k);
    std::stringstream tt;
    tt<<t;
    vs.push_back(tt.str());
    vs.push_back(v);
    return execute(vs,NULL);
}
