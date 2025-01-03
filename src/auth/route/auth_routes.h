#pragma once
#include "crow.h"
#include "auth/auth.h"
#include "crow/middlewares/cookie_parser.h"

void setupAuthRoutes(crow::App<crow::CookieParser> &app) {
    app.route_dynamic("/login")
        .methods("POST"_method)
        ([](const crow::request& req) {
            // Call the userLogin function which returns a pair of response and token
            std::pair<crow::response, std::string> login_response = userLogin(req);
            crow::response res = std::move(login_response.first); // Use std::move to avoid copying

            // If the token is not empty, add it to the Set-Cookie header
            if (!login_response.second.empty()) {
                res.add_header("Set-Cookie", "token=" + login_response.second + "; Path=/; HttpOnly; Max-Age=3600; SameSite=Strict");
            }

            // Return the response
            return res;
        });

    app.route_dynamic("/register")
        .methods("POST"_method)
        ([](const crow::request& req) {
            // Call the userRegister function which returns a crow::response
            crow::response register_response = userRegister(req);

            // Return the response
            return register_response; 
        });
}
