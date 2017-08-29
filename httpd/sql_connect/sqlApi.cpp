
#include "sqlApi.h"


sqlApi::sqlApi()
{
	conn_fd = mysql_init(NULL);
}

int sqlApi::myConnect()
{
	
	if(mysql_real_connect(conn_fd, "127.0.0.1", "root", "","Linux5_class", 3306, NULL, 0)){
		std::cout << "connect success!" << std::endl;
	}else{
		std::cout << "connect failed!" << std::endl;
	}
}

int sqlApi::mySelect()
{
	std::string sql = "SELECT * FROM st_info";
	int ret = mysql_query(conn_fd, sql.c_str());
	if(ret == 0){
		MYSQL_RES *res = mysql_store_result(conn_fd);
		if(res){
			int rows = mysql_num_rows(res);
			int fields = mysql_num_fields(res);
			std::cout << "<html>" << std::endl;
			std::cout << "rows: " << rows << " fields: " << fields << std::endl;
			MYSQL_FIELD *f;
			for(; f = mysql_fetch_field(res); ){
				std::cout << f->name << '\t';
			}
			std::cout << std::endl;

			int i, j;
			for(i=0; i < rows; i++){
				MYSQL_ROW row = mysql_fetch_row(res);
				for(j = 0; j < fields; j++){
					std::cout << row[j] << '\t';
				}
				std::cout << "<br/>" << std::endl;
			}
			std::cout << "<br/>" << std::endl;
			std::cout << "</html>" << std::endl;

			free(res);
		}
	}
}

int sqlApi::myInsert(const std::string &name,\
					 const std::string &sex,\
					 const std::string &tel,\
					 const std::string &hobby,\
					 const std::string &school)
{
	std::string sql;
	sql = "INSERT INTO st_info (name, sex, tel, hobby, school) VALUES ('";
	sql += name;
	sql += "','"; //('name','
	sql += sex;
	sql += "','"; //('name','
	sql += tel;
	sql += "','"; //('name','
	sql += hobby;
	sql += "','"; //('name','
	sql += school;
	sql += "')"; //('name','

	std::cout << sql << std::endl;

	int ret = mysql_query(conn_fd, sql.c_str());
	if(ret == 0){
		std::cout << "INSERT SUCCESS!" << std::endl;
	}
}

sqlApi::~sqlApi()
{
	mysql_close(conn_fd);
}












