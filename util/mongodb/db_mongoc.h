#ifndef _DB_MONGOC_H
#define _DB_MONGOC_H

#include <mongoc.h>
#include <stdio.h>
#include <stdlib.h>
#include "json/json.h"
#include <glog/logging.h>

class db_mongoc
{
public:
    db_mongoc(const std::string & db, const std::string & set,const std::string& time_out = "1000");
    ~db_mongoc();
    
    void add_host(const std::string & ip, const std::string& port, const std::string & user, const std::string &pwd);
   
    int  insert(const std::string & table, const Json_u::Value & data);
    
    int  query(const std::string & table, const bson_t* b, std::vector<std::string> &list, 
            uint32_t limit = 0,  const bson_t* fields = 0, uint32_t skip = 0, uint32_t batch_size = 0);
    
    int  update(const std::string & table, const bson_t* query, const bson_t* set);

    int  update(const std::string & table, const bson_t* query, const bson_t* sort, const bson_t* set, std::string & value); 

    int  remove(const std::string & table, const bson_t* b);
    
    bool  database_has_table(const std::string &table);
    
    int  create_table(const std::string & table, unsigned int size = 0, unsigned int max = 0);
    
    std::string get_error(){ return m_error; };
    int         get_code(){ return m_code; };

private:
    int  to_insert(const std::string & table, const Json_u::Value & data);
    int  to_query(const std::string & table, const bson_t* b, std::vector<std::string> &list,
            uint32_t limit = 0, const bson_t* fields = 0, uint32_t skip = 0, uint32_t batch_size = 0);
    int  to_update(const std::string & table, const bson_t* query, const bson_t* set);
    int  to_update(const std::string & table, const bson_t* query, const bson_t* sort, const bson_t* set, std::string & value);
    int  to_remove(const std::string & table, const bson_t* b);
private:
    void append_Json(bson_t* b, const std::string & n,const Json_u::Value& v);
    int  insert_array(const std::string & table, const Json_u::Value & data);
    bool  connect();
    std::string  m_time_out;   
    std::string  m_servers;
    std::string  m_replicaSet;
    mongoc_client_t *m_client;
    std::string  m_db;
    std::string  m_error;
    int          m_code;
};

class db_mongoc_clients
{
public:
    db_mongoc_clients(int num = 1);
    virtual ~db_mongoc_clients();
    virtual int AddDBCluster(std::vector<db_conf_item_tx> &dblist_,const std::string & set);
    db_mongoc* connection(const std::string& key);
    db_mongoc* connection(unsigned int index);
private:
    unsigned int  hashkey(const std::string & str_ );
    std::vector<db_mongoc*>  m_mongolist;
    int                     m_nums;
};

#endif

