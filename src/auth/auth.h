#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "crow.h"

crow::response userRegister(const crow::request &req);
std::pair<crow::response, std::string> userLogin(const crow::request &req);

#endif 
