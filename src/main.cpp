#include "crow.h"
#include "auth/auth.h"
#include "db/db_connection.h"
#include "crow/middlewares/cookie_parser.h"

int main()
{
    // crow::SimpleApp app;s
    crow::App<crow::CookieParser> app;


    CROW_ROUTE(app, "/register").methods(crow::HTTPMethod::POST)([](const crow::request &req)
    {
        crow::json::wvalue response = userRegister(req);
        return response; 
    });

    CROW_ROUTE(app, "/login").methods(crow::HTTPMethod::POST)([](const crow::request &req)
    {
        crow::json::wvalue response = userLogin(req);
        return response;
    });

    CROW_ROUTE(app, "/")
    ([]()
     {
        return "Hello world!";
    });


    CROW_ROUTE(app, "/set_cookie")
    ([](const crow::request& req) {
        crow::response res;
        res.add_header("Set-Cookie", "my_cookie=my_value; Path=/; HttpOnly");
        return res;
    });


    CROW_ROUTE(app, "/read")
    ([&](const crow::request& req) {
        auto& ctx = app.get_context<crow::CookieParser>(req);
        // Read cookies with get_cookie
        auto value = ctx.get_cookie("my_cookie");
        return "value: " + value;
    });


    app.port(18080).multithreaded().run();

    return 0;
}
