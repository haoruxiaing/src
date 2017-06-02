#ifndef _DB_MYSQL_H
#define _DB_MYSQL_H
#include <stdlib.h>
#include <utility>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <functional>
#include <sstream>
#include <iostream>
#include <mutex>

#include <mysql/mysql_version.h>
#include <mysql/mysql.h>
#include <mysql/mysqld_error.h>
#include <mysql/errmsg.h>
#include "glog/logging.h"

typedef struct{
        int          group_id;                 //群组ID
        std::string  db_;                      //数据库
        std::string  db_host_;                 //数据库ip
        std::string  port_;                    //端口
        std::string  user_;                    //用户名
        std::string  pwd_;                     //密码
        std::string  charset_;                 //字符集
        std::string  timeout_;                 //超时
}db_conf_item_t;


namespace db {
typedef std::map<std::string, std::string> StrStrMap;
typedef std::vector<StrStrMap> SQLResultSet;
typedef std::function<int (uint64_t)> myfunc;
class db_mysql
{
public:
    db_mysql(){ m_isConnected = false; m_autocommit=false; m_index = 0; };
    virtual ~db_mysql(){ };
    void AddServer(db_conf_item_t & t)
    {
        db_conf_item_t * st = new db_conf_item_t;
        st->group_id = t.group_id;
        st->db_ = t.db_;
        st->db_host_ = t.db_host_;
        st->port_ = t.port_;
        st->user_ = t.user_;
        st->pwd_ = t.pwd_;
        st->charset_ = t.charset_;
        st->timeout_ = t.timeout_;
        m_server.push_back(st);
    };
    // 执行SQL语句
    //
    // 执行以后, 可以调用GetUpdateCount()获取最后insert的sequence id.
    //
    // @return success true
    //         fail    false
    bool Execute(const std::string& sql,int cnt = 0);
    
    // 执行查询语句
    //
    // 执行成功, 结果存储在results中, 每行结果是一个vector, vector中
    // 存pair,first是列名, second是value.
    // 
    // @return success true
    //         fail    false
    bool ExecuteQuery(const std::string& sql, SQLResultSet& results);
    
    // 执行更新语句
    // 
    // @return success 0
    //         fail    -1
    //         
    int ExecuteUpdate(const std::string& sql);

    // 返回最后影响的行数
    uint64_t GetInsertId() { 
        return mysql_insert_id(&m_handle);
    }
    int get_error(){ return m_error; }
    std::string EscapeString(const std::string& s);
    virtual int autocommit();
    virtual int commit();
    virtual int rollback();
    int connect();
private:
    std::vector<db_conf_item_t*> m_server;
    MYSQL                        m_handle;
    bool                         m_isConnected;
    bool                         m_autocommit;
    int                          m_index;
    int                          m_error;
    time_t                       m_connect_time;
};

class db_SqlStream
{
public:
    db_SqlStream(db_mysql*c){
        conn = c;
    };
    ~db_SqlStream(){
    };
    std::string  str(){ return sql.str(); };
    db_SqlStream & operator<<(int v){ sql<<v; return *this; };
    db_SqlStream & operator<<(int64_t v) { sql<<v; return *this;  };
    db_SqlStream & operator<<(unsigned int v){ sql<<v; return *this;  };
    db_SqlStream & operator<<(uint64_t v){ sql<<v; return *this;  };
    db_SqlStream & operator<<(const std::string & v){ sql<<conn->EscapeString(v); return *this; };
    db_SqlStream & operator<<(const char * v){ sql<<v; return *this; };
private:
    std::stringstream sql;
    db_mysql *        conn;
};

class db_mysqlclients
{
public:
    db_mysqlclients(int num = 1)
    {
        m_nums = num;		
        m_mysqllist = std::vector<db_mysql*> (m_nums, NULL);
    };
    ~db_mysqlclients()
    {
        for(uint32_t i = 0;i < m_mysqllist.size(); i++){
            if (m_mysqllist[i] != NULL){
                delete m_mysqllist[i];
                m_mysqllist[i] = NULL;
            }
        }
    };
    virtual int AddDBCluster( std::vector<db_conf_item_t> &dblist_ )
    {
        for ( unsigned int i = 0; i < m_nums; i++ ){
            m_mysqllist[i] = new db_mysql;
            if( NULL == m_mysqllist[i] ) return -1;
            for( unsigned int j = 0; j < dblist_.size(); j++ ){
                if( (int)i == dblist_[j].group_id){
                     m_mysqllist[i]->AddServer(dblist_[j]);
                }
            }
        }
        for ( unsigned int i = 0; i < m_nums; i++ ){
            if (m_mysqllist[i] != NULL){
                m_mysqllist[i]->connect();
            }
        }
        return 0;
    };
    db_mysql *  GetConnection(uint64_t uid, const myfunc & fun)
    {
        unsigned int index = fun(uid);
        index--;
        if( index >= m_nums ){
            LOG(ERROR)<<"GetConnection false uid["<<uid<<"] index["<<index<<"]";
            return NULL;
        }
        return m_mysqllist[index];
    }
    db_mysql * GetConnection(int index)
    {
        index--;
        if ((unsigned int)index >= m_nums ){
            LOG(ERROR)<<"GetConnection  index["<<index<<"] false";
            return NULL;
        }
        return m_mysqllist[index];
    }
    void ReleaseConnection(uint64_t uid, db_mysql* conn, const myfunc& index){};
    std::vector<db_mysql*>  m_mysqllist;
    unsigned int            m_nums;
};
}
#endif

