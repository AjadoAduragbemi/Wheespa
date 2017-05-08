
#include "dbapi.hpp"


namespace dbapi{

	void FlexDB::open(const std::string& db_file_name){
		setLAT("");
		#ifdef _WIN32
			SQLITE_OPEN sqlite3_open = (SQLITE_OPEN) GetProcAddress(lib_handle, "sqlite3_open");
		#endif

		if(sqlite3_open(db_file_name.c_str(),
						&db_handle) != SQLITE_OK)
			throw std::runtime_error("An error occured while trying to open file");
	}


	int FlexDB::close(){
		#ifdef _WIN32
			SQLITE_CLOSE sqlite3_close = (SQLITE_CLOSE) GetProcAddress(lib_handle, "sqlite3_close");
		#endif

		return sqlite3_close(db_handle);
	}


	bool FlexDB::insert(const std::string& table,
						const std::string& columns,
						const std::vector<std::string>& values){
		db_quote_params = true;

		last_func_called = "db_insert";

		db_query = "insert into " + table + "(" +
				   columns + ") values(" +
				   vec_to_str(values) + ")";
		
		setLAT(table);

		return execQuery(db_query);
	}

	
	const std::string& FlexDB::getLAT() const{
		return this->lat;
	}


	std::string FlexDB::vec_to_str(const std::vector<std::string>& vec){
		std::string str = "", append_to_str;
		std::vector<std::string>::const_iterator _iter;
    
		for(_iter = vec.begin(); _iter != vec.end(); _iter++){

			if(db_quote_params)

				if(is_all_digit(*_iter)){
					append_to_str = *_iter;
				}else{
					append_to_str = "\"" + *_iter + "\"";
				}
				
			else append_to_str = *_iter;
        
			str.append(append_to_str + ",");
		}
    
		str.pop_back(); /* remove trailing comma */
		db_quote_params = false;
		
		return str;
	}

	
	bool FlexDB::setTableStructure(const std::string& table){		
		#ifdef _WIN32
			SQLITE_GET_TABLE sqlite3_get_table = (SQLITE_GET_TABLE) GetProcAddress(lib_handle, "sqlite3_get_table");
		#endif
		
		ptable get = new table_st;
		
		std::string cmd = "select * from " + table;
		
		last_func_called = "db_init_table";
		
		if(sqlite3_get_table( db_handle,
							  cmd.c_str(),
							  &get->table_buf,
							  &get->row_count,
							  &get->col_count,
							  &db_errMsg) == SQLITE_ERROR){

			db_an_error_occured = true;

		}else{

			setLAT(table);
        
			get->rows = new std::vector<std::string>[get->col_count];
			
			for(int i = 0; i < get->col_count; i++){

				get->columns.push_back(get->table_buf[i]);

				for(int j = i + get->col_count, k = 0;
					k < get->row_count;
					j += get->col_count, k++){
                
					get->rows[i].push_back(get->table_buf[j]);
				}
			}
						
			db_an_error_occured = false;
		}
    
		db_table = db_an_error_occured ? nullptr : get;

		return db_an_error_occured ? false : true;
	}

	
	void FlexDB::printTable(const std::string& table){

		if(table == "") setTableStructure(lat);
		else setTableStructure(table);
    
		if(db_table != nullptr){
			std::cout << db_table;
		}else{
			printErrMsg();
		}
	}

	
	bool FlexDB::deleteRecord(const std::string& table,
							  const std::string& where){
		last_func_called = "db_delete_record";

		db_query = "delete from " + table + " where ";
		db_query += where;

		setLAT(table);
		return execQuery(db_query);
	}

	
	bool FlexDB::dropTable(const std::string& table){
		last_func_called = "db_drop_table";
		db_query = "drop table " + table;
    
		setLAT(table);
    
		return execQuery(db_query);
	}

	
	ptable FlexDB::getTableStructure() const{
		return this->db_table;
	}

	
	std::vector<std::string> FlexDB::getColumns(std::string table){
		if(table == "") table = lat;

		#ifdef _WIN32
			SQLITE_COL_NAME sqlite3_column_name = (SQLITE_COL_NAME) GetProcAddress(lib_handle, "sqlite3_column_name");
		#endif

		last_func_called = "db_get_columns";

		std::vector<std::string> columns;

		if(initializeStmt("select * from " + table)){

			for(int i = 0; i < getColumnCount(); i++){
				
				columns.push_back(std::string(sqlite3_column_name(db_stmt, i)));

			}
		}

		setLAT(table);
    
		return columns;
	}

	
	bool FlexDB::initializeStmt(const std::string& sql_code){
		#ifdef _WIN32
			SQLITE_PREPARE sqlite3_prepare_v2 = (SQLITE_PREPARE) GetProcAddress(lib_handle, "sqlite3_prepare_v2");
		#endif

		last_func_called = "db_init_stmt";
    
		if(sqlite3_prepare_v2(db_handle,
							  sql_code.c_str(),
							  sql_code.size(),
							  &db_stmt,
							  nullptr) != SQLITE_OK){

			db_an_error_occured = true;
		}else{
			db_an_error_occured = false;
		}
    
		return db_an_error_occured ? false : true ;
	}

	
	sqlite3_stmt *FlexDB::getStmt() const{
		return this->db_stmt;
	}

	
	int FlexDB::getColumnCount(){
		#ifdef _WIN32
			SQLITE_COL_CNT sqlite3_column_count = (SQLITE_COL_CNT) GetProcAddress(lib_handle, "sqlite3_column_count");
		#endif
		
		last_func_called = "db_get_column_count";

		if(this->db_stmt == nullptr) return -1;		

		return sqlite3_column_count(db_stmt); 
	}
	

	bool FlexDB::update(const std::string& table,
						const std::string& set,
						const std::string& where){
		last_func_called = "db_update";
    
		db_query = "update " + table + " set " + set;
		db_query += " where " + where;
    
		setLAT(table);
		return execQuery(db_query);
	}


	bool FlexDB::createTable(const std::string& table,
							 const std::vector<std::string>& colmns){

		last_func_called = "db_create_table";
		db_quote_params = false;
    
		db_query = "create table if not exists " +
				   table + "(";

		db_query += vec_to_str(colmns) + ")";
    
		setLAT(table);
    
		return execQuery(db_query);
    
	}


	bool FlexDB::execQuery(const std::string& query,
						   int(*c_func)(void*,int,char**,char**),
						   void *arg){

		#ifdef _WIN32
			SQLITE_EXEC sqlite3_exec = (SQLITE_EXEC) GetProcAddress(lib_handle, "sqlite3_exec");
		#endif


		if(sqlite3_exec(db_handle,
						query.c_str(),
						c_func,
						arg,
						&db_errMsg) != SQLITE_OK){

			db_an_error_occured = true;
		}else{
			db_an_error_occured = false;
		}

		return !db_an_error_occured;
	}

	
	void FlexDB::printErrMsg(){
		std::cerr << getLastErrMsg();
	}

	
	std::string FlexDB::getLastErrMsg() const{
		std::ostringstream ss;

		if(db_errMsg == nullptr) return "\n";

		ss << "error(" + last_func_called + "): "
			<< db_errMsg << std::endl;

		return db_an_error_occured ? ss.str() : "\n";
	}


	bool FlexDB::insert(const std::string& columns,
						std::vector<std::string>& values){
		return insert(lat, columns, values);
	}

	
	bool FlexDB::deleteRecord(const std::string& where){
		return deleteRecord(lat, where);
	}


	bool FlexDB::update(const std::string& set,
						const std::string& where){
		return update(lat, set, where);
	}


	bool is_all_digit(const std::string& str){
		std::string::const_iterator ch = str.begin();
    
		while(ch != str.end()){
			if(isdigit(*ch));
			else break;
        
			*ch++;
		}
    
		return (ch == str.end()) ? true : false;
	}
}

