
#include "dbapi.hpp"

namespace dbapi{
	/* Draw the table */
	std::ostream& operator <<(std::ostream& out,
							  const ptable& table){
		if(table == nullptr) return out;

		//int table_buf_size = (table->col_count*(table->row_count+1));
		
		std::vector<int> col_draw;
		
		for(int i = 0; i < table->col_count; i++){
			size_t col_draw_size = 0;
			if(col_draw_size < table->columns[i].length()) col_draw_size = table->columns[i].length();
			for(int j = 0; j < table->row_count; j++){
				if(col_draw_size < table->rows[i][j].length()) col_draw_size = table->rows[i][j].length();
			}
			col_draw.push_back(col_draw_size);
			
		}
		
		/* a lambda function to define center */
		auto center = [](const std::string& in, size_t size, char fill_char=' '){
			std::string out = in;
			if(out.length() >= size) return out;
			
			size_t fill = size - out.length();
			
			out.insert(0, fill/2, fill_char);
			out.append(fill/2, fill_char);
			
			return out;
		};
		
		/* table head */
		out << " | ";
		for(int i = 0; i < table->col_count; i++){
			std::string str = table->columns[i];
			if(str.length() > col_draw[i]) str = str.substr(0, col_draw[i]);
			else{
				str = center(str, col_draw[i]);
				str.append(col_draw[i]-str.length(), ' ');
			}
			out << str << " | ";
		}
		out << std::endl;
		
		/* table body */
		for(int i = 0; i < table->row_count; i++){
			out << " | ";
			for(int j = 0; j < table->col_count; j++){
				std::string str = table->rows[j][i];
				if(str.length() > col_draw[j]) str = str.substr(0, col_draw[j]);
				else{
					str.append(col_draw[j]-str.length(), ' ');
				}
				out << str << " | ";
			}
			out << std::endl;
		}
		out << std::endl;
		
		return out;
	}
}