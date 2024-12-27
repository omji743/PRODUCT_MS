#include "crow.h"
#include "auth/auth.h"
#include "db/db_connection.h"
#include "crow/middlewares/cookie_parser.h"
#include <string>
#include "product/Product.h"
#include "product/ProductDatabase.h"
#include <vector>
#include "auth/authMiddleware.h"

int main()
{
    // crow::SimpleApp app;
    crow::App<crow::CookieParser> app;

    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)([](const crow::request &req)
                                                                 {
        crow::json::wvalue response = userRegister(req);
        return response; });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([](const crow::request &req)
                                                              {
    std::pair<crow::json::wvalue, std::string> response = userLogin(req);
    crow::response res;

    if (!response.second.empty()) {
        res.add_header("Set-Cookie", "token=" + response.second + "; Path=/; HttpOnly; Max-Age=3600; SameSite=Strict");
    }
    res.body = response.first.dump(); 
    return res; });

    CROW_ROUTE(app, "/add-product")
    ([&](const crow::request &req)
     {
         std::pair<bool, std::string> user = authMiddleware(req, app);
         if (user.first)
         {
             return crow::response(200, user.second); // 200 OK with the user email
         }
         else
         {
             return crow::response(401, "Unauthorized"); // 401 Unauthorized response
         }
     });

    app.port(18080).multithreaded().run();

    return 0;
}
