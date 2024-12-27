#ifndef PRODUCTDATABASE_H
#define PRODUCTDATABASE_H

#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <vector>
#include "Product.h"

class ProductDatabase {
private:
    sql::mysql::MySQL_Driver *driver;
    sql::Connection *con;

public:
    ProductDatabase();
    ~ProductDatabase();

    void add_product(const Product& product);
    std::vector<Product> get_all_products();
    Product get_product_by_id(int id);
    void update_product(const Product& product);
    void delete_product(int id);
};

#endif