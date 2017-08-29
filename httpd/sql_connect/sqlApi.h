
#ifndef _SQL_API_H_
#define _SQL_API_H_

#include <iostream>
#include <mysql.h>

class sqlApi{
	public:
		sqlApi();
		int myConnect();
		int mySelect();
		int myInsert(const std::string &name,\
					 const std::string &sex,\
					 const std::string &tel,\
					 const std::string &hobby,\
					 const std::string &school);
		~sqlApi();
		//int myUpdate();
		//int myDelete();
	private:
		MYSQL *conn_fd;
};

#endif
