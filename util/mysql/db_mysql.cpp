#include "db_mysql.h"

std::mutex   m_mysql_mtx;

std::string db::db_mysql::EscapeString(const std::string& s)
{
    if (m_isConnected && mysql_ping(&m_handle) != 0){
        m_isConnected = false;
        mysql_close(&m_handle);
    }
    if (!m_isConnected){
       if (connect() < 0){
            return s;
       }
    }
    std::vector<char> buffer(s.size() * 2 + 1);
    unsigned long return_len = mysql_real_escape_string(&m_handle, buffer.data(), s.c_str(), s.size());
    return std::string(buffer.data(), return_len);
}

int db::db_mysql::autocommit()
{
    if (m_isConnected && mysql_ping(&m_handle) != 0)
    {
        m_isConnected = false;
        mysql_close( &m_handle );
    }
    if (connect() < 0){
        return -1;
    }
    mysql_autocommit(&m_handle,0);
    m_autocommit = true;
    return 0;
}
int db::db_mysql::commit()
{
    int rt = 0;
    try{
         mysql_commit(&m_handle);
    }catch(...)
    {
         LOG(ERROR)<<"DB["<<m_server[m_index]->db_host_<<"] commit error["<<mysql_errno(&m_handle)<<"]:"<<mysql_error(&m_handle);
         rt = -1;
    }
    m_autocommit = false;
    mysql_autocommit(&m_handle,1);
    return rt;
}
int db::db_mysql::rollback()
{
    int rt = 0;
    try{
        mysql_rollback(&m_handle);
    }catch(...)
    {
        LOG(ERROR)<<"DB["<<m_server[m_index]->db_host_<<"]rollback error["<<mysql_errno(&m_handle)<<"]:"<<mysql_error(&m_handle);
        rt = -1;
    }
    m_autocommit = false;
    mysql_autocommit(&m_handle,1);
    m_isConnected = false;
    mysql_close( &m_handle );
    return rt;
}

int db::db_mysql::connect()
{
    if (m_isConnected || m_autocommit){
        return 0;
    }
    for( unsigned int i = 0; i <m_server.size(); i++ )
    {
        int cindex = (i+m_index)%m_server.size();
        mysql_init( &m_handle );
        {
            std::lock_guard<std::mutex> _lock(m_mysql_mtx);
            LOG(INFO)<<"DB connect ["<<m_server.size()<<"] index["<<cindex<<"]"<<" host["<<m_server[cindex]->db_host_<<"]"<<" charset:"<<m_server[cindex]->charset_.c_str();
            mysql_options ( &m_handle, MYSQL_SET_CHARSET_NAME, m_server[cindex]->charset_.c_str() );//设置连接字符集        
            int timeout = 0;
            if(m_server[cindex]->timeout_.length() > 0 ) timeout = atoi(m_server[cindex]->timeout_.c_str());
            if ( 0 != timeout ){
                mysql_options ( &m_handle, MYSQL_OPT_CONNECT_TIMEOUT , (const char*)(&timeout) );
                mysql_options ( &m_handle, MYSQL_OPT_READ_TIMEOUT , (const char*)(&timeout) );
                mysql_options ( &m_handle, MYSQL_OPT_WRITE_TIMEOUT , (const char*)(&timeout) );
            }
        }
        int port = 0;
        if(m_server[cindex]->port_.length() > 0 ){
            port = atoi(m_server[cindex]->port_.c_str());
        }
        try{
            if ( !mysql_real_connect(&m_handle, m_server[cindex]->db_host_.c_str(), m_server[cindex]->user_.c_str(),
                                          m_server[cindex]->pwd_.c_str(), m_server[cindex]->db_.c_str(),port, NULL, 0) )
            {
                int errorno = mysql_errno(&m_handle);
                LOG(ERROR)<<"connect::"<<m_server[cindex]->db_host_.c_str()<<","<<m_server[cindex]->user_.c_str()<<","<<m_server[cindex]->db_.c_str()<<"false["<<errorno<<"]:"<<mysql_error(&m_handle);
                continue;
            }
        }
        catch(...){
            int errorno = mysql_errno(&m_handle);
            LOG(ERROR)<<"connect::"<<m_server[cindex]->db_host_.c_str()<<","<<m_server[cindex]->user_.c_str()<<","<<m_server[cindex]->db_.c_str()<<"false["<<errorno<<"]:"<<mysql_error(&m_handle);
            continue;
        }
        m_index = cindex;
        m_isConnected = true;
        m_connect_time = time(0);
        LOG(WARNING)<<"DB connect ["<<m_server.size()<<"] index["<<cindex<<"]"<<" host["<<m_server[cindex]->db_host_<<"] ok";
        return 0;
    }
    return -1;
}

bool db::db_mysql::Execute(const std::string& sql_,int cn)
{
    if ( sql_.empty() ) return false;
    if ( cn == 4 ) return false;
    if ( !m_isConnected ){
        if ( connect() < 0 ){
            return false;
        }
    }
    if (m_index != 0 && (time(0) - m_connect_time) >= 3 ){ //3秒尝试连接主数据库
        m_isConnected = false;
        mysql_close( &m_handle );
        m_index = 0;
        if (connect()< 0){ return false; }
    }
    if ( !m_isConnected )return -1;
    int _state = mysql_real_query ( &m_handle, sql_.c_str(), sql_.size() );
    if ( 0 != _state ){
        int errorno = mysql_errno(&m_handle);
        if ( CR_SERVER_GONE_ERROR == errorno        //The server gone away;
                 || CR_SERVER_LOST == errorno 
                 || 1158 == errorno || 1159 == errorno 
                 || 1160 == errorno || 1161 == errorno )          //Lost connection to MySQL server during query 
        {
            m_isConnected = false;
            mysql_close( &m_handle );
            if ( m_autocommit )
            {
                LOG(ERROR)<<"m_autocommit false";
                return false;  //m_autocommit 直接失败
            }
            LOG(ERROR)<<"mysql_real_query["<<sql_<<"] ERROR["<<mysql_errno(&m_handle)<<"]:"<<mysql_error(&m_handle);
            return Execute(sql_,++cn); //重执行SQL语句。
        }
        else{
            m_error = mysql_errno(&m_handle);
            LOG(ERROR)<<"DB["<<m_server[m_index]->db_host_<<"] Execute failed SQL["
                      <<sql_.c_str()<<"] ERROR["<<mysql_errno(&m_handle)<<"]:"<<mysql_error(&m_handle);
            return false;
        }
    }
    return true;
}
bool db::db_mysql::ExecuteQuery(const std::string& sql, SQLResultSet& results)
{
    if (!Execute(sql)){
        return false;
    }
    bool exe_res = false;
    do {
        MYSQL_RES* res = mysql_store_result(&m_handle);
        int _fieldnum = (int)mysql_field_count (&m_handle);
        if (!res && 0 != _fieldnum ) {
            LOG(ERROR) <<"DB["<<m_server[m_index]->db_host_ <<"] use result error: " << mysql_error(&m_handle)<<" :"<<mysql_error(&m_handle);
            break;
        }
        if ( !res ) return true;
        MYSQL_ROW _row = mysql_fetch_row ( res );
        MYSQL_FIELD * _fieldinfo = mysql_fetch_fields ( res );
        if ( !_row ){
             mysql_free_result(res);
             return true;
        }
        unsigned long *lengths = mysql_fetch_lengths( res );
        for(;;)
        {
            if(!_row){break;}
            StrStrMap strmap;
            for( int i = 0; i < _fieldnum; ++i)
            {
                auto p = std::make_pair(_fieldinfo[i].name,
                                        std::string(_row[i], (int)lengths[i]));
                strmap.insert(p);
            }
            results.push_back(strmap);
            _row = mysql_fetch_row( res);
            lengths = mysql_fetch_lengths( res);
        }
        mysql_free_result(res);
        exe_res = true;
    }while (0);
    return exe_res;
}
int  db::db_mysql::ExecuteUpdate(const std::string& sql)
{
    if (!Execute(sql,2)){
        return -1;
    }
    return mysql_affected_rows(&m_handle);
}


