#ifndef DBAPI_HPP
#define DBAPI_HPP


#include"sqlite3.h"
	

#include<vector>
#include<list>
#include<iostream>
#include<cstring>
#include<set>
#include<sstream>
#include<stdexcept>


#ifdef _WIN32
	#include<Windows.h>
	typedef INT (_cdecl *SQLITE_CLOSE)(sqlite3*);
	typedef INT (_cdecl *SQLITE_OPEN)(LPCSTR, sqlite3**);
	typedef INT (_cdecl *SQLITE_EXEC)(sqlite3*, LPCSTR, INT(*callback)(LPVOID,INT, LPSTR*, LPSTR*), LPVOID, LPSTR*);
	typedef LPCSTR (_cdecl *SQLITE_COL_NAME)(sqlite3_stmt*, INT);
	typedef INT (_cdecl *SQLITE_COL_CNT)(sqlite3_stmt*);
	typedef INT (_cdecl *SQLITE_PREPARE)(sqlite3*, LPCSTR, INT, sqlite3_stmt**, LPCSTR*);
	typedef INT (_cdecl *SQLITE_GET_TABLE)(sqlite3*, LPCSTR, LPSTR**, LPINT, LPINT, LPSTR*);

#endif



namespace dbapi{

	typedef struct table_st{

		std::vector<std::string> *rows;
		std::vector<std::string> columns;
		int row_count;
		int col_count;
		char **table_buf;
    
	} *ptable;

		
	class FlexDB{
		sqlite3 *db_handle;
		sqlite3_stmt *db_stmt;
    
		char* db_errMsg;

		std::string db_query,
			   last_func_called,
			   last_accessed_table;

		bool db_an_error_occured, db_quote_params;

		std::vector<std::string> columns;

		ptable db_table;
    
		std::string lat; /* last accessed table */

		#if defined(_WIN32)
			HMODULE lib_handle;
		#endif

		void setLAT(const std::string lat){
			this->lat = lat;
		}
        
		std::string vec_to_str(const std::vector<std::string>&);
    
		public:
			 
			FlexDB() :  db_errMsg(nullptr),
						db_stmt(nullptr),
						db_handle(nullptr),
						db_table(nullptr),
						db_quote_params(false),
						db_an_error_occured(false){
				#if defined(_WIN32)
					lib_handle = LoadLibraryA("sqlite3.dll");
				#endif
			}


			FlexDB(const std::string db_file_name) : db_errMsg(nullptr),
												db_stmt(nullptr),
												db_handle(nullptr),
												db_table(nullptr),
												db_quote_params(false),
												db_an_error_occured(false){
				#if defined(_WIN32)
					lib_handle = LoadLibraryA("sqlite3.dll");
				#endif

				open(db_file_name);
			}

			~FlexDB(){
				if(db_table != nullptr) {
					delete[] db_table->rows;
				}
				//free(db_stmt);
				close();
			}
        
			void open(const std::string&);
			
			int close();
        
			bool execQuery(const std::string&,
						   int(*)(void*,int,char**,char**) = nullptr,
						   void * = nullptr);
        
			bool createTable(const std::string&,
							 const std::vector<std::string>&);
        
			bool insert(const std::string&,
						const std::string&,
						const std::vector<std::string>&);
			bool insert(const std::string&,
						std::vector<std::string>&);
    
			bool deleteRecord(const std::string&,
							  const std::string&);
			bool deleteRecord(const std::string&);
        
			bool update(const std::string&,
						const std::string&,
						const std::string&);
			bool update(const std::string&, const std::string&);
        
			bool dropTable(const std::string&);
        
			
			bool setTableStructure(const std::string&);
        
			ptable getTableStructure() const;
        
			std::vector<std::string> getColumns(std::string = "");

			/* initStmt must have been called */
			int getColumnCount();
        
			bool initializeStmt(const std::string& );
        
			sqlite3_stmt *getStmt() const;

			void printTable(const std::string& = "");
        
			void printErrMsg();
        
			std::string getLastErrMsg() const;
        
			const std::string& getLAT() const;
	};

	
	
	std::ostream& operator << (std::ostream& out,
							   const ptable& get);

	bool is_all_digit(const std::string&);

}


#endif


