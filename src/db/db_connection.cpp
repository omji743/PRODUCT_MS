#include "db_connection.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/connection.h>

sql::Connection* getDbConnection()
{
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

    try
    {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "your_password");
        con->setSchema("ims");
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        throw;
    }

    return con;
}
