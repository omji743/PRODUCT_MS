#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "crow.h"

crow::json::wvalue userRegister(const crow::request &req);
crow::json::wvalue userLogin(const crow::request &req);

#endif // USER_AUTH_H
