#include "couch_client.h"
#include <unistd.h>
#include "json/json.h"

struct cach_cookie
{
    int       error;
    std::map<std::string, couch_value> *  value;
};

static void bootstrap_callback(lcb_t instance, lcb_error_t error)
{
    cach_cookie* s = (cach_cookie*)lcb_get_cookie(instance);
    if (error == LCB_SUCCESS) 
    {
        return;
    }
    if(s)
    {
       s->error = -1;
    }
}

static void store_callback(lcb_t instance, const void *cookie,lcb_storage_t operation,lcb_error_t error,const lcb_store_resp_t *item)
{
    if (error == LCB_SUCCESS)
    {
        return;
    }
    if(cookie)
    {
        cach_cookie * s = (cach_cookie*)cookie;
        s->error = -1;
        if (s->value)
        {
            std::string key((const char*)item->v.v0.key,item->v.v0.nkey);
            couch_value v;
            (*(s->value))[key] = v;
        }
    }
    LOG(ERROR)<<"store_callback false:"<<error;
}

static void get_callback(lcb_t instance, const void *cookie, lcb_error_t error, const lcb_get_resp_t *item)
{
    if (error == LCB_SUCCESS)
    {
        if(cookie)
        {
            cach_cookie * s = (cach_cookie*)cookie;
            s->error = 1;
            if(s->value)
            {
                std::string key((const char*)item->v.v0.key,item->v.v0.nkey);
                couch_value v;
                v.value.clear();
                v.value.append((const char*)item->v.v0.bytes,item->v.v0.nbytes);
                v.cas = item->v.v0.cas;
                (*(s->value))[key] = v;
            }
        }
        return;
    }
    if (error == LCB_KEY_ENOENT)
    {
        if(cookie)
        {
            cach_cookie * s = (cach_cookie*)cookie;
            s->error = 0;
        }
        return;
    }
    if(cookie)
    {
        LOG(ERROR)<<"get_callback error:"<<error;
        cach_cookie * s = (cach_cookie*)cookie;
        s->error = -1;
    }
}
static void remove_callback(lcb_t instance,const void *cookie,lcb_error_t error,const lcb_remove_resp_t *resp)
{
    if ( error == LCB_SUCCESS )
    {
        return;
    }
    if (error == LCB_KEY_ENOENT)
    {
        if(cookie)
        {
            cach_cookie * s = (cach_cookie*)cookie;
            s->error = 0;
        }
        return;
    }
    if(cookie)
    {
        cach_cookie * s = (cach_cookie*)cookie;
        s->error = -1;
    }
}
static void unlock_callback(lcb_t instance,const void *cookie, lcb_error_t error, const lcb_unlock_resp_t *resp)
{
    if ( error == LCB_SUCCESS )
    {
        if (cookie){
            ((cach_cookie*)cookie)->error = 0;
        }   
    }else
    {
        if (cookie){
            ((cach_cookie*)cookie)->error = -1;
        }
    }
}

couch_client::couch_client(){ m_connect = false; };
couch_client::~couch_client()
{
    if (m_connect)
    {
        lcb_wait(m_instance);
        lcb_destroy(m_instance);
    }
};
int  couch_client::connect(float t)
{
    lcb_error_t err;
    struct lcb_create_st create_options;
    memset(&create_options, 0, sizeof(create_options));
    create_options.version = 3;
    std::stringstream sd;
    sd<<"couchbase://"<<m_bucket.c_str();
    create_options.v.v3.connstr = sd.str().c_str();
    err = lcb_create(&m_instance, &create_options);
    lcb_set_bootstrap_callback(m_instance, bootstrap_callback);
    if (err != LCB_SUCCESS){
        return err;
    }
    cach_cookie cookie;
    cookie.error = 0;
    lcb_set_cookie(m_instance,&cookie);
    std::stringstream ot;
    ot<<t;
    lcb_cntl_string(m_instance, "operation_timeout", ot.str().c_str());
    err = lcb_connect(m_instance);
    if (err != LCB_SUCCESS) {
        lcb_destroy(m_instance);
        return err;
    }
    lcb_wait(m_instance);
    (void)lcb_set_get_callback(m_instance, get_callback);
    (void)lcb_set_store_callback(m_instance, store_callback);
    (void)lcb_set_remove_callback(m_instance, remove_callback);
    (void)lcb_set_unlock_callback(m_instance, unlock_callback);
    m_connect = true;
    return cookie.error;
}
int  couch_client::connect(std::string & bucket, float t)
{
    m_bucket = bucket;
    try
    {
        return connect(t);
    }
    catch(...)
    {
        return -1;
    }
};
int couch_client::set(const std::string& key, const std::string& value) 
{
    std::map<std::string,std::string> mp;
    mp[key] = value;
    return  sets(&mp);
};
int couch_client::sets( std::map<std::string,std::string> * value)
{
    if (setxs(value, 0) == 0)
    {
        return 0;
    }
    return setxs(value, 0);
}
int couch_client::setx( const std::string& key, const std::string& value,int alivetime )
{
    std::map<std::string,std::string> mp;
    mp[key] = value;
    if (setxs(&mp, alivetime)==0)
    {
        return 0;
    }
    return setxs(&mp,alivetime);
};
int couch_client::setxs( std::map<std::string,std::string> * value,int alivetime )
{
    lcb_error_t err;
    lcb_store_cmd_t *commands[value->size()];
    int index = 0;
    std::map<std::string,std::string>::iterator it = value->begin();
    for( ; it!=value->end(); it++ )
    {
        lcb_store_cmd_t* cmd = new lcb_store_cmd_t;
        memset(cmd, 0, sizeof(lcb_store_cmd_t));
        cmd->v.v0.operation = LCB_SET;
        cmd->v.v0.key = it->first.c_str();
        cmd->v.v0.nkey = it->first.length();
        cmd->v.v0.bytes = it->second.c_str();
        cmd->v.v0.nbytes = it->second.length();
        cmd->v.v0.exptime = alivetime;
        commands[index] = cmd;
        index++;
    }
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = 0;
    err = lcb_store(m_instance, &cookie, index, commands);
    lcb_wait(m_instance);
    for(int i = 0; i < index; i++)
    {
        lcb_store_cmd_t* cmd = commands[i];
        delete cmd;
    }
    if ( err != LCB_SUCCESS ) {
        LOG(ERROR)<<"setxs false:"<<err;
        return -1;
    }
    return cookie.error;
}
int couch_client::get(const std::string& key, std::string* value)
{
    std::vector<std::string> keys;
    keys.push_back(key);
    std::map<std::string,std::string> map;
    int rt = gets(&keys,&map);
    if (rt<0){
        rt = gets(&keys,&map);
    }
    if (map.size()>0)
    {
        value->append(map[key]);
    }
    return rt;
};
int couch_client::gets( std::vector<std::string>* keys, std::map<std::string,std::string> * value )
{
    lcb_error_t err;
    lcb_get_cmd_t cmd;
    lcb_get_cmd_t *commands[keys->size()];
    int index = 0;
    for(unsigned int i = 0; i < keys->size(); i++)
    {
        lcb_get_cmd_t * cmd = new lcb_get_cmd_t;
        memset(cmd, 0, sizeof(lcb_get_cmd_t));
        cmd->v.v0.key = (*keys)[i].c_str();
        cmd->v.v0.nkey = (*keys)[i].length();
        commands[index] = cmd;
        index++;
    }
    std::map<std::string, couch_value> s;
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = &s;
    err = lcb_get(m_instance, &cookie, index, commands);
    lcb_wait(m_instance);
    for(int i = 0; i < index; i++)
    {
        lcb_get_cmd_t* cmd = commands[i];
        delete cmd;
    }
    if (err != LCB_SUCCESS) {
        LOG(ERROR)<<"gets false:"<<err;
        return -1;
    }
    std::map<std::string, couch_value>::iterator it = s.begin();
    for (;it!=s.end();it++){
        std::string k = it->first;
        std::string v = it->second.value;
        (*value)[k] = v;
    }
    return cookie.error;
}
int couch_client::del( const std::string & key)
{
    std::vector<std::string> keys;
    keys.push_back(key);
    if (dels(&keys)==0)
    {
        return 0;
    }
    return dels(&keys);
}
int couch_client::dels( std::vector<std::string> * keys )
{
    lcb_error_t err;
    lcb_remove_cmd_t* commands[keys->size()];
    int index = 0;
    for(unsigned int i=0;i<keys->size();i++)
    {
        lcb_remove_cmd_t * cmd = new lcb_remove_cmd_t;
        cmd->version = 0;
        cmd->v.v0.key =(*keys)[i].c_str();
        cmd->v.v0.nkey = (*keys)[i].length();
        //cmd->v.v0.cas = 0x666;
        commands[index] = cmd;
        index++;
    }
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = 0;
    err = lcb_remove(m_instance, &cookie, index, commands);
    lcb_wait(m_instance);
    for(int i = 0; i < index; i++)
    {
        lcb_remove_cmd_t* cmd = commands[i];
        delete cmd;
    }
    if( err != LCB_SUCCESS ){
        return -1;
    }
    return cookie.error;
}
/*
unsigned int couch_uint(unsigned char *a_pvBytes)
{
    unsigned int nRet = a_pvBytes[0];
    for(int i=1; i<4; i++){
        nRet <<= 8;
        nRet |= a_pvBytes[i];
    }
    return nRet;
}
void couch_bytes(unsigned int a_iVal, unsigned char *a_pvBytes)
{
    for(int i=4-1; i>=0; i--){
        a_pvBytes[i] = a_iVal & 0xFF;
        a_iVal >>= 8;
    }
}*/

bool parse_one(const char * &point, unsigned int &len, std::string & key, std::string &v)
{
    if (len<sizeof(int)) {return false;}
    unsigned int * vl = (unsigned int* )point;
    point += sizeof(int);
    len -= sizeof(int);
    if (len < *vl ) {return false;}
    key.append(point, *vl);
    
    point += (*vl);
    len -= (*vl);
    if (len<sizeof(int)){return false;}
    
    vl = (unsigned int* )point;
    point += sizeof(int);
    len -= sizeof(int);
    if (len < *vl ){return false;}
    v.append(point, *vl);
    
    point += (*vl);
    len -= (*vl);
    
    return true;
}
int couch_client::hget(const std::string &name, const std::string &key, std::string *val)
{
    std::string value;
    if (get(name,&value)<0)
    {
        return -1;
    }
    std::vector<std::string> keys;
    std::vector<std::string> vals;
    unsigned int len = value.length();
    const char * point = value.c_str();
    for(unsigned int i = 0; i < value.length(); i++)
    {
        std::string _tkey, _tvs;
        if (!parse_one(point, len, _tkey, _tvs)){ break; }
        if (_tkey == key){
            val->append(_tvs);
            return 1;
        }
    }
    return 0;
}
int couch_client::hset(const std::string &name, const std::string &key, const std::string &val)
{
    int try_cnt = 0;
    do{
        std::string value;
        uint64_t cas = 0;
        int rt = get_lock(name, value, cas);
        try_cnt++;
        if (rt < 0){
            if (try_cnt > 5){
                LOG(ERROR)<<"eget_lock["<<name<<"] false";
                return -1;
            }
            LOG(WARNING)<<"wget_lock["<<name<<"] false";
            continue;
        }
        if (rt == 0) {
            std::string s_value = "0";
            if (set(name,s_value)<0) 
            {
                LOG(ERROR)<<"set["<<name<<"] 0 false";
                return -1;
            }
            continue;
        }
        std::vector<std::string> keys;
        std::vector<std::string> vals;
        unsigned int len = value.length();
        unsigned int clen = len;
        const char * point = value.c_str();
        bool has = false;
        for(unsigned int i = 0; i < clen; i++){
            std::string _tkey, _tvs;
            if (!parse_one(point, len, _tkey, _tvs)){ break; }
            if (_tkey == key)
            {
                has = true;
                vals.push_back(val);
            }else{
                vals.push_back(_tvs);
            }
            keys.push_back(_tkey);
        }
        std::string set_value;
        for(unsigned int i = 0; i < vals.size(); i++){
            int klen = keys[i].length();
            set_value.append((const char*)&klen,sizeof(int));
            set_value.append(keys[i]);
            int vlen = vals[i].length();
            set_value.append((const char*)&vlen,sizeof(int));
            set_value.append(vals[i]);
        }
        if(!has){
            int klen = key.length();
            set_value.append((const char*)&klen,sizeof(int));
            set_value.append(key);
            int vlen = val.length();
            set_value.append((const char*)&vlen,sizeof(int));
            set_value.append(val);
        }
        if (set_unlock(name, set_value, cas)<0){
            if (try_cnt > 5){
                LOG(ERROR)<<"eset_unlock["<<name<<"] false";
                return -9;     //lock false
            }
            LOG(WARNING)<<"wset_unlock["<<name<<"] false";
            continue;
        }
        return 0;
    }while(true);
    return 0;
}
int couch_client::hdel(const std::string &name, const std::string &key)
{
    int try_cnt = 0;
    do{
        std::string value;
        uint64_t cas = 0;
        int rt = get_lock(name, value, cas);
        try_cnt++;
        if (rt < 0){
            if (try_cnt > 5){
                LOG(ERROR)<<"get_lock["<<name<<"] false";
                return -1;
            }
            LOG(WARNING)<<"get_lock["<<name<<"] false";
            continue;
        }
        if (rt == 0) { return 0; }
        std::vector<std::string> keys;
        std::vector<std::string> vals;
        unsigned int len = value.length();
        const char * point = value.c_str();
        for (unsigned int i = 0; i < value.length(); i++){
            std::string _tkey, _tvs;
            if (!parse_one(point, len, _tkey, _tvs)){ break; }
            if (_tkey != key){
                vals.push_back(_tvs);
                keys.push_back(_tkey);
            }
        }
        std::string set_value;
        for (unsigned int i = 0; i < vals.size(); i++){
            int klen = keys[i].length();
            set_value.append((const char*)&klen,sizeof(int));
            set_value.append(keys[i]);
            int vlen = vals[i].length();
            set_value.append((const char*)&vlen,sizeof(int));
            set_value.append(vals[i]);
        }
        if (set_value.length()<=0){set_value = "0";}
        if (set_unlock(name, set_value, cas)<0){
            if (try_cnt > 5)
            {
                LOG(ERROR)<<"get_lock["<<name<<"] false";
                return -9;     //lock false
            }
            continue;
        }
        return 0;
    }while(true);
    return -1;
}
int couch_client::hscan(const std::string &name, const std::string &key_start, const std::string &key_end,uint64_t limit, std::vector<std::string> *ret )
{
    std::string value;
    if (get(name,&value)<0){
        return -1;
    }
    std::vector<std::string> keys;
    unsigned int len = value.length();
    const char * point = value.c_str();
    for (unsigned int i = 0; i < value.length(); i++){
        std::string _tkey, _tvs;
        if (!parse_one(point, len, _tkey, _tvs)){ break; }
        ret->push_back(_tvs);
    }
    return 0;
}
int couch_client::hscans(std::vector<std::string> *names, std::map<std::string, std::vector<std::string> > &rets)
{
    std::map<std::string,std::string> map;
    int rt = gets(names, &map);
    std::map<std::string,std::string>::iterator it = map.begin();
    for( ; it!=map.end(); it++ ){
        std::string key = it->first;
        unsigned int len = it->second.length();
        const char * point = it->second.c_str();
        for (unsigned int i = 0; i < it->second.length(); i++){
            std::string _tkey, _tvs;
            if (!parse_one(point, len, _tkey, _tvs)){ break; }
            rets[it->first].push_back(_tvs);
        }
    }
    return rt;
}
int couch_client::hkeys(const std::string &name, std::vector<std::string> *ret)
{
    std::string value;
    if (get(name,&value)<0){
        return -1;
    }
    unsigned int len = value.length();
    const char * point = value.c_str();
    for(unsigned int i = 0; i < value.length(); i++){
        std::string _tkey, _tvs;
        if (!parse_one(point, len, _tkey, _tvs)){ break; }
        ret->push_back(_tkey);
    }
    return 0;
}
struct q_cookie
{
    lcb_cas_t cas;
    int error;
    std::vector<std::string> * data;
};
static void rowCallback(lcb_t instance, int cbtype, const lcb_RESPN1QL *resp)
{
    q_cookie * cookie = (q_cookie*)resp->cookie;
    if (! (resp->rflags & LCB_RESP_F_FINAL) ) {
        std::string data(resp->row,resp->nrow);
        Json_u::Value s;
        Json_u::Reader read;
        if ( read.parse(data.c_str(),s))
        {
            Json_u::FastWriter w;
            std::string sw =w.write(s);
            sw[sw.size()-1] = '\0';
            if (cookie){
                cookie->data->push_back(sw);
            }
        }
    } else {
        std::string data(resp->row,resp->nrow);
        Json_u::Value s;
        Json_u::Reader read;
        if ( read.parse(data.c_str(),s))
        {
            if (s["status"].asString() == "errors")
            {
                printf("Got metadata: %.*s\n", (int)resp->nrow, resp->row);
                if (cookie){ cookie->error = -1; }
            }
        }
    }
}
int couch_client::query(lcb_CMDN1QL &cmd, std::vector<std::string> *ret)
{
    cmd.callback = rowCallback;
    q_cookie ck;
    ck.error = 0;
    ck.data = ret;
    lcb_error_t rc = lcb_n1ql_query(m_instance, &ck, &cmd);
    lcb_wait(m_instance);
    if (rc != LCB_SUCCESS) {
        return -1;
    }
    return ck.error;
}
int couch_client::unlock(const std::string &key, uint64_t cas )
{
    cach_cookie c;
    lcb_unlock_cmd_t *unlock =  new lcb_unlock_cmd_t;
    unlock->version = 0;
    unlock->v.v0.key = key.c_str();
    unlock->v.v0.nkey = key.length();
    unlock->v.v0.cas = cas;
    lcb_unlock_cmd_t* commands[] = { unlock };
    lcb_unlock(m_instance,&c, 1, commands);
    lcb_wait(m_instance);
    return c.error;
}
int couch_client::get_lock(const std::string& key, std::string& value, uint64_t &cas, int timeout)
{
    std::vector<std::string> keys;
    keys.push_back(key);
    std::map<std::string,couch_value> v;
    if (get_locks(keys,&v)<0)
    {
        return -1;
    }
    std::map<std::string,couch_value>::iterator it = v.find(key);
    if (it!=v.end()){
        value = it->second.value;
        cas = it->second.cas;
        return 1;
    }
    return 0;
}
int couch_client::get_locks(const std::vector<std::string>& keys, std::map<std::string,couch_value>* value)
{
    lcb_error_t err;
    lcb_get_cmd_t cmd;
    lcb_get_cmd_t *commands[keys.size()];
    int index = 0;
    for(unsigned int i = 0; i < keys.size(); i++)
    {
        lcb_get_cmd_t * cmd = new lcb_get_cmd_t;
        memset(cmd, 0, sizeof(lcb_get_cmd_t));
        cmd->v.v0.key = keys[i].c_str();
        cmd->v.v0.nkey = keys[i].length();
        commands[index] = cmd;
        index++;
    }
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = value;
    err = lcb_get(m_instance, &cookie, index, commands);
    lcb_wait(m_instance);
    for(int i = 0; i < index; i++)
    {
        lcb_get_cmd_t* cmd = commands[i];
        delete cmd;
    }
    if (err != LCB_SUCCESS) {
        LOG(ERROR)<<"get_locks false:"<<err;
        return -1;
    }
    return cookie.error;
}
int couch_client::set_unlock(const std::string& key, std::string& value, uint64_t cas)
{
    lcb_error_t err;
    lcb_store_cmd_t* cmd = new lcb_store_cmd_t;
    lcb_store_cmd_t * commands[] = {cmd};
    memset(cmd, 0, sizeof(lcb_store_cmd_t));
    cmd->v.v0.operation = LCB_REPLACE;
    cmd->v.v0.key = key.c_str();
    cmd->v.v0.nkey = key.length();
    cmd->v.v0.bytes = value.c_str();
    cmd->v.v0.nbytes = value.length();
    cmd->v.v0.cas = cas;
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = 0;
    err = lcb_store(m_instance, &cookie, 1, commands);
    lcb_wait(m_instance);
    delete cmd;
    if (err != LCB_SUCCESS) {
        LOG(ERROR)<<"set_unlock false:"<<err;
        return -1;
    }
    return cookie.error;
}
int couch_client::set_unlocks(std::map<std::string, couch_value> *value, std::map<std::string, couch_value> *error)
{
    lcb_error_t err;
    lcb_store_cmd_t *commands[value->size()];
    int index = 0;
    std::map<std::string, couch_value>::iterator it = value->begin();
    for (; it!=value->end(); it++)
    {
        lcb_store_cmd_t* cmd = new lcb_store_cmd_t;
        memset(cmd, 0, sizeof(lcb_store_cmd_t));
        cmd->v.v0.operation = LCB_SET;
        cmd->v.v0.key = it->first.c_str();
        cmd->v.v0.nkey = it->first.length();
        cmd->v.v0.bytes = it->second.value.c_str();
        cmd->v.v0.nbytes = it->second.value.length();
        cmd->v.v0.cas = it->second.cas;
        commands[index] = cmd;
        index++;
    }
    cach_cookie cookie;
    cookie.error = 0;
    cookie.value = error;
    err = lcb_store(m_instance, &cookie, index, commands);
    lcb_wait(m_instance);
    for(int i = 0; i < index; i++)
    {
        lcb_store_cmd_t* cmd = commands[i];
        delete cmd;
    }
    if (err != LCB_SUCCESS) {
        LOG(ERROR)<<"set_unlocks false:"<<err;
        return -1;
    }
    return cookie.error;
}

