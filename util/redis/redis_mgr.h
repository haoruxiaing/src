#ifndef __REDIS_MGR_H__
#define __REDIS_MGR_H__

#include "util/redis/redis_clients.h"
#include "base/cache_base.h"

class cache_handler_redis :public cache_base
{
public:
    cache_handler_redis(int nums = 1,int db_number = 0){
        m_dbNumber = db_number;
        m_nums = nums;
        all_clients = std::vector<redis_clients*> (m_nums, NULL);
    };
    ~cache_handler_redis(){
        for(uint32_t i = 0;i < all_clients.size(); i++){
            redis_clients *pClient = all_clients[i];
            if (pClient != NULL){
                delete pClient;
                all_clients[i] = NULL;
            }
        }
    }
    virtual int  Init( std::vector<cache_conf_item_t>& list_, bool need_master_check = false )
    {
         for ( int i = 0; i < m_nums; i++ ){
             all_clients[i] = new redis_clients(m_dbNumber);
             if( !all_clients[i] ) return -1;
             for( unsigned int j = 0; j <list_.size(); j++ ){
                 if(i == list_[j].group_id){
                    all_clients[i]->add_host(list_[j].host, list_[j].port);
                    all_clients[i]->set_time_out(list_[j].timeout);
                 }
             }
         }
         return 0;
    };
    int   select( int db ){
        if( NULL == all_clients[0] )return -1;
        return all_clients[0]->select(db);
    };
    /*
    @key: 缓存的key 唯一标示
    @value: 存放的数据
    @alivetime: 存活时间
    */
    virtual int expire(const std::string &key_, int64_t timeout)
    {
        unsigned int index = hashkey(key_);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->expire(  key_,  timeout ); 
    };
    
    virtual int set( const std::string& key_, const std::string& value_ ) 
    {
        unsigned int index = hashkey(key_);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->set(  key_,  value_ ); 
    };
    
    virtual int setx( const std::string& key_, const std::string& value_, int alivetime_ ) 
    {
        unsigned int index = hashkey(key_);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->setx(  key_,  value_,alivetime_ ); 
    };
    
    virtual int get( const std::string& key_, std::string* value_ )
    {
        unsigned int index = hashkey(key_);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->get(  key_,  value_ ); 
    };
    
    virtual int del( const std::string& key_ )
    {
        unsigned int index = hashkey(key_);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->del( key_ ); 
    };
    
    virtual int multi_get(const std::vector<std::string> &keylist_, std::vector<std::string> * vallist)
    {
        unsigned int index = 0;
        std::vector<std::string> ss;
        for(unsigned int i = 0; i < keylist_.size(); i++ ){
            unsigned int in = hashkey(keylist_[i]);
            if( i == 0 ){
                 index = in;
                 ss.push_back(keylist_[i]);
                 continue;
            }
            if( index == in ){
                 ss.push_back(keylist_[i]);
                 continue;
            }
            if( all_clients[index] )all_clients[index]->multi_get( ss, vallist);
            ss.clear();
            index = in;
            ss.push_back(keylist_[i]);
        }
        if( all_clients[index] ) all_clients[index]->multi_get( ss, vallist);
        return 0;
    };
    
    virtual int multi_exists(const std::vector<std::string>&keylist_, std::vector<std::string> *vallist )
    {
        std::vector< std::vector<std::string> > _keylist;
        for(int i = 0; i < m_nums; i++ )
        {
            std::vector<std::string> s1;
            _keylist.push_back(s1);
        }
        for(unsigned int i = 0; i < keylist_.size(); i++ )
        {
            unsigned int index = hashkey(keylist_[i]);
            _keylist[index].push_back(keylist_[i]);	
        }
        for(int i = 0; i < m_nums; i++ )
        {
            if( NULL == all_clients[i] ) continue;
            all_clients[i]->multi_exists( _keylist[i], vallist);
        }
        return 0;
    };
    
    /*
    @hash methods
    */
    virtual int multi_hexists(const std::string &name, const std::vector<std::string> &keylist, std::vector<std::string> * vallist)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->multi_hexists( name,keylist, vallist);
    };

    virtual int multi_hget(const std::string &name, const std::vector<std::string> &keylist, std::vector<std::string> * vallist)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->multi_hget( name,keylist, vallist);
    };

    virtual int hget(const std::string &name, const std::string &key, std::string *val)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;
        return all_clients[index]->hget( name,key, val); 
    };

    virtual int hset(const std::string &name, const std::string &key, const std::string &val)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hset( name,key, val); 
    };	
    
    virtual int hdel(const std::string &name, const std::string &key)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hdel( name,key); 
    };	

    virtual int hincr(const std::string &name, const std::string &key, int64_t incrby, int64_t *ret)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hincr( name,key,incrby,ret); 
    };	

    virtual int hsize(const std::string &name, int64_t *ret) 
    {
        //unsigned int index = hashkey(name);
        return 0;//all_clients[index]->hsize( name,ret); 
    }

    virtual int hclear(const std::string &name)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hclear( name ); 
    };

    virtual int hscan(const std::string &name, const std::string &key_start, const std::string &key_end,uint64_t limit, std::vector<std::string> *ret)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hscan( name,key_start,key_end,limit, ret); 
    };

    virtual int hkeys(const std::string &name, const std::string&key_start, const std::string&key_end, uint64_t limit, std::vector<std::string> *ret)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hkeys( name,key_start,key_end,limit, ret); 
    };

    virtual int hkeys(const std::string &name, std::vector<std::string> *ret)
    {
        unsigned int index = hashkey(name);
        if( NULL == all_clients[index] )return -1;		
        return all_clients[index]->hkeys( name, ret); 
    };
    
    virtual int qpush_back(const std::string& name, const std::string& qname, const std::string& qitem, int64_t* ret) 
    {
        unsigned int index = hashkey(name);
        if (all_clients[index] == nullptr) return -1;
        return all_clients[index]->qpush_back(qname, qitem, ret);
    }
    virtual int qpush(const std::string& name, const std::string& k, const std::vector<std::string>& v)
    {
        unsigned int index = hashkey(name);
        if (all_clients[index] == nullptr) return -1;
        return all_clients[index]->qpush(k, v);
    }
    virtual int qpushx(const std::string& name, const std::string& k, const std::vector<std::string>& v)
    {
        unsigned int index = hashkey(name);
        if (all_clients[index] == nullptr) return -1;
        return all_clients[index]->qpushx(k, v);
    }
    virtual int qtrim(const std::string& name, const std::string& k, uint32_t start, uint32_t end){
        unsigned int index = hashkey(name);
        if (all_clients[index] == nullptr) return -1;
        return all_clients[index]->qtrim(k, start, end);
    }
    virtual int qrange(const std::string& name, const std::string &k, uint32_t start, uint32_t end, std::vector<std::string> & rt){
        unsigned int index = hashkey(name);
        if (all_clients[index] == nullptr) return -1;
        return all_clients[index]->qrange(k, start, end, rt);
    }

private:
    unsigned int  hashkey(const std::string & str_ )
    {
        const char* key = str_.c_str();
        register size_t hash = 0;  
        while (size_t ch = (size_t)*key++)  
        {         
            hash = hash * 131 + ch;   // 也可以乘以31、131、1313、13131、131313..     
        }
        return hash%m_nums;
    }

private:
    std::vector<redis_clients*> all_clients;
    int  m_nums;
    int  m_dbNumber;
};

#endif
