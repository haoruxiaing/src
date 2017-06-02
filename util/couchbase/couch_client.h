#ifndef _COUCH_CLIENT_H
#define _COUCH_CLIENT_H

#include <libcouchbase/couchbase.h>
#include <libcouchbase/n1ql.h>
#include <glog/logging.h>
#include <map>

struct couch_value
{
    std::string value;
    uint64_t    cas;
};

class couch_client
{
public:
    couch_client();
    virtual ~couch_client();
    /*
     *  172.16.0.149:11210,172.16.0.152:11210,172.16.0.153:11210/bucket 50ms
     *  ok(0) false(-1) 
     */
    int  connect(std::string & addr, float op_time_out = 0.03);

    /*
     *  ok(0) false(-1)
     */
    virtual int set( const std::string& key, const std::string& value);

    /*
     *  set list ok(0) false(-1)
     */
    virtual int sets( std::map<std::string,std::string> * value);

    /*
     *  ok(0) false(-1)
     */
    virtual int setx( const std::string& key, const std::string& value,int alivetime );
    
    /*
     *  setx list ok(0) false(-1)
     */
    virtual int setxs( std::map<std::string,std::string> * value,int alivetime );

    /*
     *  ok(1) not have(0) false(-1)
     */
    virtual int get( const std::string& key, std::string* value );

    /*
     *  get list ok(1) not have(0) false(-1)
     */
    virtual int gets( std::vector<std::string>* keys, std::map<std::string,std::string> * value);

    /*
     *  ok(0) false(-1)
     */
    virtual int del( const std::string& key );

    /*
     *  del list ok(0) false(-1)
     */
    virtual int dels( std::vector<std::string>* keys );

    /*
     *  ok(0) false(-1)
     */
    virtual int hget(const std::string &name, const std::string &key, std::string *val);
    virtual int hset(const std::string &name, const std::string &key, const std::string &val);
    virtual int hdel(const std::string &name, const std::string &key);
    virtual int hscan(const std::string &name, const std::string &key_start, const std::string &key_end,uint64_t limit, std::vector<std::string> *ret );
    virtual int hscans(std::vector<std::string> *names, std::map<std::string, std::vector<std::string> > &rets);
    virtual int hkeys(const std::string &name, std::vector<std::string> *ret);
    virtual int query(lcb_CMDN1QL &sql, std::vector<std::string> *ret);
    virtual int unlock(const std::string& key, uint64_t cas);
    virtual int get_lock(const std::string& key, std::string& value, uint64_t &cas, int timeout = 0);
    virtual int get_locks(const std::vector<std::string>& keys, std::map<std::string, couch_value> *value);
    virtual int set_unlock(const std::string& key, std::string& value, uint64_t cas);
    virtual int set_unlocks(std::map<std::string, couch_value> *value, std::map<std::string, couch_value> *error);
private:
    int  connect(float t);
    std::string              m_bucket;
    lcb_t                    m_instance;
    lcb_cas_t                m_cas;
    bool                     m_connect;
};

#endif

