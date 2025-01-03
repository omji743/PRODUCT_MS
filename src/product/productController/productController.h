#ifndef PRODUCT_CONTROLLER_H
#define PRODUCT_CONTROLLER_H
#include "crow.h"

crow::response getAllProducts();

crow::response addProduct(const crow::request& req,std::string user_email);

crow::response getProductById(const crow::request& req, int id);

crow::response updateProductById(const crow::request& req, int id,const std::string& user_email);

crow::response deleteProductById(int id,const std::string& user_email);

crow::response getAvailableStock(const crow::request& req,std::string user_email);
#endif