#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "crow.h"

crow::json::wvalue userRegister(const crow::request &req);
std::pair<crow::json::wvalue,std::string> userLogin(const crow::request &req);

#endif 
