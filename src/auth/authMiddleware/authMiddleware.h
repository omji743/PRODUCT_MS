#ifndef AUTHMIDDLEWARE_H
#define AUTHMIDDLEWARE_H

#include "crow/middlewares/cookie_parser.h"
#include "crow.h"


std::pair<bool,std::string> authMiddleware(const crow::request &req,crow::App<crow::CookieParser> &app);

#endif