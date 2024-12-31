#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include "db/db_connection.h"
#include "isAdmin.h"

bool isAdmin(const std::string &user_email) {
    sql::Connection* con = getDbConnection();  
    if (!con) {
        std::cerr << "Failed to connect to the database" << std::endl;
        return false;
    }

    try {
        sql::PreparedStatement *stmt = con->prepareStatement("SELECT role FROM users WHERE email = ?");
        stmt->setString(1, user_email); 
        sql::ResultSet *res = stmt->executeQuery();

        if (!res->next()) {
            delete stmt;
            delete res;
            delete con;
            return false;
        }

        std::string role = res->getString("role");
        delete stmt;
        delete res;
        delete con;

        return role == "admin"; 
    } catch (sql::SQLException &e) {
        std::cerr << "Database error: " << e.what() << std::endl;
        delete con;
        return false;
    }
}
