#include "db_mongoc.h"

db_mongoc_clients::db_mongoc_clients(int num)
{
    m_nums = num;
    m_mongolist = std::vector<db_mongoc*> (m_nums, NULL);
}
db_mongoc_clients::~db_mongoc_clients()
{
    for (uint32_t i = 0;i < m_mongolist.size(); i++){
        db_mongoc *pClient = m_mongolist[i];
        if (pClient != NULL){
            delete pClient;
            m_mongolist[i] = NULL;
        }
    }
}

int db_mongoc_clients::AddDBCluster(std::vector<db_conf_item_tx> &dblist_,const std::string & set)
{
    for (int i = 0; i < m_nums; i++){
        for (unsigned int j = 0; j <dblist_.size(); j++){
            if (!m_mongolist[i]){
                m_mongolist[i] = new db_mongoc(dblist_[j].db_, set, dblist_[j].timeout_);
            }
            if(i == dblist_[j].group_id){
                LOG(INFO)<<"i:"<<i<<" add host:"<<dblist_[j].db_host_;
                m_mongolist[i]->add_host(dblist_[j].db_host_, dblist_[j].port_, dblist_[j].user_, dblist_[j].pwd_);
            }
        }
    }
    return 0;
}
db_mongoc* db_mongoc_clients::connection(const std::string& key) 
{
    uint32_t index = hashkey(key);
    if (m_mongolist[index]) 
        return m_mongolist[index];
    return 0;
}
db_mongoc* db_mongoc_clients::connection(unsigned int index)
{
    //index从 1开始
    if (index==0 || (int)index >m_nums){ return 0; }
    return m_mongolist[index-1];
}
unsigned int db_mongoc_clients::hashkey(const std::string & str_ )
{
    const char* key = str_.c_str();
    register size_t hash = 0;  
    while (size_t ch = (size_t)*key++)  
    {         
        hash = hash * 131 + ch;   // 也可以乘以31、131、1313、13131、131313..     
    }
    return hash%m_nums;
}

db_mongoc::db_mongoc(const std::string & db, const std::string & set, const std::string & time_out)
{
    mongoc_init();
    m_servers.clear();
    m_replicaSet = set;
    m_db = db;
    if (atol(time_out.c_str())<100){
        m_time_out = "100";
    }else{
        m_time_out = time_out;
    }
    m_client = 0;
}
db_mongoc::~db_mongoc(){
    if (m_client){
        mongoc_client_destroy(m_client);
        m_client = 0;
    }
    mongoc_cleanup();
}
void db_mongoc::add_host(const std::string & ip, const std::string& port, const std::string & user, const std::string & pwd){
    if (m_servers.length()<=0){
        if (user.length()>0 && pwd.length()>0){
            m_servers = "mongodb://"+user+":"+ pwd +"@"+ip+":"+port;
        }else{
            m_servers = "mongodb://"+ip+":"+port;
        }
    }else{
        m_servers = m_servers + "," + ip+ ":" + port;
    }
}

int  db_mongoc::insert(const std::string & table, const Json_u::Value & data)
{
    int rt = 0;
    for (int i=0; i<2; i++){
        rt = to_insert(table,data);
        if (rt<0 && (m_code == 59)){ continue; }
        break;
    }
    return rt;
}
int  db_mongoc::query(const std::string & table, const bson_t* b, std::vector<std::string> &list,
            uint32_t limit, const bson_t* fields, uint32_t skip, uint32_t batch_size)
{
    int rt = 0;
    for (int i=0; i<2; i++){
        rt = to_query(table, b, list, limit, fields, skip, batch_size);
        if (rt<0 && (m_code == 59) && list.size()<=0){ continue; }
        break;
    }
    return rt;
}
int  db_mongoc::update(const std::string & table, const bson_t* query, const bson_t* set)
{
    int rt = 0;
    for (int i=0; i<2; i++){
        rt = to_update(table, query, set);
        if (rt<0 && (m_code == 59)){ continue; }
        break;
    }
    return rt;
}
int  db_mongoc::update(const std::string & table, const bson_t* query, const bson_t* sort, const bson_t* set, std::string & value)
{
    int rt = 0;
    for (int i=0; i<2; i++){
        rt = to_update(table, query, sort, set, value);
        if (rt<0 && (m_code == 59)){ continue; }
        break;
    }
    return rt;
}
int  db_mongoc::remove(const std::string & table, const bson_t* b)
{
    int rt = 0;
    for (int i=0; i<2; i++){
        rt = to_remove(table, b);
        if (rt<0 && (m_code == 59)){ continue; }
        break;
    }
    return rt;
}

int  db_mongoc::to_insert(const std::string & table, const Json_u::Value & data)
{
    if (data.type() == Json_u::arrayValue)
    {
        return insert_array(table, data);
    }else{
        Json_u::Value tb;
        tb.append(data);
        return insert_array(table, tb);   
    }
    return 0;
}
int  db_mongoc::to_query(const std::string & table, const bson_t* b, std::vector<std::string> &list, uint32_t limit, const bson_t* fields, uint32_t skip, uint32_t batch_size)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    int rt = 0;
    bson_error_t error;
    const bson_t *doc;
    char *str;
    mongoc_collection_t *coll = mongoc_client_get_collection (m_client, m_db.c_str(), table.c_str());
    mongoc_cursor_t* cursor = mongoc_collection_find (coll, MONGOC_QUERY_NONE, skip, limit, batch_size, b, fields, NULL);
    while (mongoc_cursor_more (cursor) && mongoc_cursor_next (cursor, &doc)) {
        str = bson_as_json (doc, NULL);
        std::string data = str;
        list.push_back(data);
        bson_free (str);
    }
    if (mongoc_cursor_error(cursor, &error)) {
        m_error.clear();
        m_error = error.message; m_code = error.code;
        LOG(ERROR)<<"mongoc_collection_find error:"<<error.code<<" :"<<error.message;
        if (error.code == 59 || error.code == 13053){
            mongoc_client_destroy (m_client);
            m_client = 0;
        }
        rt = -1;
    }
    mongoc_cursor_destroy (cursor);
    mongoc_collection_destroy(coll);
    return rt;
}
int db_mongoc::to_update(const std::string & table, const bson_t* query, const bson_t* set)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    int rt = 0;
    bson_error_t error;
    mongoc_write_concern_t *wc = mongoc_write_concern_new();
    mongoc_write_concern_set_wtimeout(wc, 500);
    mongoc_collection_t *coll = mongoc_client_get_collection (m_client, m_db.c_str(), table.c_str());
    if (!mongoc_collection_update(coll, MONGOC_UPDATE_MULTI_UPDATE, query, set, wc, &error)){
        m_error.clear();
        m_error = error.message; m_code = error.code;
        LOG(ERROR)<<"mongoc_collection_update false:"<<error.code<<" :"<<error.message;
        if (error.code == 59 || error.code == 13053){
            mongoc_client_destroy (m_client);
            m_client = 0;
        }
        rt = -1;
    }
    mongoc_write_concern_destroy (wc);
    mongoc_collection_destroy(coll);
    return rt;
}
int db_mongoc::to_update(const std::string & table, const bson_t* query, const bson_t* sort, const bson_t* set, std::string & value)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    int rt = 0;
    bson_error_t error;
    bson_t reply; bson_init(&reply);
    mongoc_collection_t *coll = mongoc_client_get_collection(m_client, m_db.c_str(), table.c_str());
    if (!mongoc_collection_find_and_modify (coll, query, sort, set, 0, false, false, false, &reply, &error)){
        m_error.clear();
        m_error = error.message; m_code = error.code;
        LOG(ERROR)<<"mongoc_collection_find_and_modify false:"<<error.code<<" :"<<error.message;
        if (error.code == 59 || error.code == 13053 ) //not master connect time out
        {
            mongoc_client_destroy (m_client);
            m_client = 0;
        }
        rt = -1;
    }else{
        value = bson_as_json(&reply,0);
    }
    bson_destroy(&reply);
    mongoc_collection_destroy(coll);
    return rt;
}
int db_mongoc::to_remove(const std::string & table, const bson_t* b)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    int rt = 0;
    bson_error_t error;
    mongoc_write_concern_t *wc = mongoc_write_concern_new();
    mongoc_write_concern_set_wtimeout(wc, 500);
    mongoc_collection_t *coll = mongoc_client_get_collection (m_client, m_db.c_str(), table.c_str());
    if (!mongoc_collection_remove (coll, MONGOC_REMOVE_NONE, b, wc, &error)){
        m_error.clear();
        m_error = error.message; m_code = error.code;
        LOG(ERROR)<<"mongoc_collection_remove false:"<<error.code<<" :"<<error.message;
        if (error.code == 59 || error.code == 13053) //not master
        {
            mongoc_client_destroy (m_client);
            m_client = 0;
        }
        rt = -1;
    }
    mongoc_write_concern_destroy (wc);
    mongoc_collection_destroy(coll);
    return rt;
}
void db_mongoc::append_Json(bson_t* b, const std::string & n,const Json_u::Value& v)
{
    if (v.type() == Json_u::stringValue){
        std::string sd = v.asString();
        bson_append_utf8(b, n.c_str(), n.length(), sd.c_str(), sd.length());
        return;
    }
    if (v.type() == Json_u::booleanValue){
        bson_append_bool(b, n.c_str(), n.length(), v.asInt());
        return;
    }
    if (v.type() == Json_u::intValue || v.type() == Json_u::uintValue || v.type() == Json_u::realValue)
    {
        bson_append_int64(b, n.c_str(), n.length(), v.asInt64());
        return;
    }
    Json_u::FastWriter w;
    std::string sd = w.write(v);
    bson_append_utf8(b, n.c_str(), n.length(), sd.c_str(), sd.length());
}
int  db_mongoc::insert_array(const std::string & table, const Json_u::Value & data)
{
    int rt = 0;
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    mongoc_collection_t *coll = mongoc_client_get_collection (m_client, m_db.c_str(), table.c_str());
    unsigned int i = 0;
    mongoc_write_concern_t *wc = mongoc_write_concern_new();
    mongoc_write_concern_set_wtimeout(wc, 500);
    do{
        bson_t  b[1024];
        bson_t *bptr[1024];
        int num = 0;
        for (int j = 0; j<1024; j++)
        {
            bson_init(&b[j]);
            Json_u::Value::Members s = data[i].getMemberNames();
            for (unsigned int k=0; k<s.size(); k++){
                append_Json(&b[j], s[k], data[i][s[k]]);
            }
            bptr[j] = &b[j];
            i++; num++;
            if (i>=data.size()){break;}
        }
        bson_error_t error;
        if (!mongoc_collection_insert_bulk (coll, MONGOC_INSERT_NONE, (const bson_t **)bptr, num, wc, &error)){
            m_error.clear();
            m_error = error.message; m_code = error.code;
            LOG(ERROR)<<"mongoc_collection_insert_bulk false:"<<error.code<<" :"<<error.message;
            if (error.code == 59 || error.code == 13053) //not master
            {
                mongoc_client_destroy (m_client);
                m_client = 0;
            }
            rt = -1;
        }
        for (int j=0; j<num; j++){ bson_destroy(&b[j]); }
        if (rt==-1){break;}
        if (i>=data.size()){break;}
    }while(true);
    mongoc_write_concern_destroy (wc);
    mongoc_collection_destroy(coll);
    return rt;
}
bool  db_mongoc::connect()
{
    if (!m_client){
        //std::string str = m_servers + "/"+ m_db +"?replicaSet=" + m_replicaSet+ "&sockettimeoutms="+m_time_out;
        std::string str = m_servers + "/?authSource=" + m_db + "&replicaSet=" + m_replicaSet + "&sockettimeoutms="+m_time_out + "&connecttimeoutms="+m_time_out;
        LOG(WARNING)<<str;
        m_client = mongoc_client_new(str.c_str());
        return m_client ? true :false;
    }
    return true;
}
bool  db_mongoc::database_has_table(const std::string &table)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return false; }
    mongoc_database_t* database = mongoc_client_get_database(m_client, m_db.c_str());
    bson_error_t error;
    bool rt = mongoc_database_has_collection(database, table.c_str(), &error);
    if (!rt){
        LOG(ERROR)<<"has collection error:"<<error.domain<<" "<<error.code<<" "<<error.message;
    }
    mongoc_database_destroy(database);
    return rt;
}
int  db_mongoc::create_table(const std::string & table, unsigned int size, unsigned int max)
{
    if (!connect()){ LOG(ERROR)<< "connect false"; return -1; }
    mongoc_database_t* database = mongoc_client_get_database(m_client, m_db.c_str());
    int rt = 0;
    bson_t capped_options = BSON_INITIALIZER;
    if (size || max){
        BSON_APPEND_BOOL (&capped_options, "capped", true);
        BSON_APPEND_INT32 (&capped_options, "size", size);
        BSON_APPEND_INT32 (&capped_options, "max", max);
    }
    bson_error_t error;
    mongoc_collection_t * c = mongoc_database_create_collection (database, table.c_str(), &capped_options, &error);
    if (!c){
        rt = -1;
        LOG(ERROR)<<"create_collection error:"<<error.domain<<" "<<error.code<<" "<<error.message;
    }else{
        mongoc_collection_destroy(c);
    }
    bson_destroy(&capped_options);
    mongoc_database_destroy(database);
    return rt;
}
