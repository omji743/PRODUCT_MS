#pragma once
#include "crow.h"
#include "auth/auth.h"
#include "crow/middlewares/cookie_parser.h"



void setupAuthRoutes(crow::App<crow::CookieParser> &app) {
    app.route_dynamic("/login")
        .methods("POST"_method)
        ([](const crow::request& req) {

    std::pair<crow::json::wvalue, std::string> response = userLogin(req);
    crow::response res;

    if (!response.second.empty()) {
        res.add_header("Set-Cookie", "token=" + response.second + "; Path=/; HttpOnly; Max-Age=3600; SameSite=Strict");
    }
    res.body = response.first.dump(); 
    return res;
        });

    app.route_dynamic("/register")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::json::wvalue response = userRegister(req);
    return response; 
        });
}