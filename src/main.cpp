#include "crow.h"
#include "auth/auth.h"
#include "db/db_connection.h"
#include "crow/middlewares/cookie_parser.h"
#include <string>
#include "product/Product.h"
#include "product/ProductDatabase.h"
#include <vector>
#include "auth/authMiddleware/authMiddleware.h"
#include "product/productController/productController.h"

int main()
{
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



    // Add product
    CROW_ROUTE(app, "/add-product").methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req)
     {
         std::pair<bool, std::string> user = authMiddleware(req, app);
         if (user.first)
         {
             crow::response res=addProduct(req,user.second);
                return res;
         }
         else
         {
             return crow::response(401, "Unauthorized"); // 401 Unauthorized response
         }
     });


    // Get all products
    CROW_ROUTE(app, "/get-products")([]() {

        crow::json::wvalue resp = getAllProducts();
        return crow::response(200, resp);
    });

    // Get a single product by ID
    CROW_ROUTE(app, "/products/<int>")([](const crow::request& req, int id) {
        crow::response res=getProductById(req,id);
        return res;
    });

    // Update a product
CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::PUT)
([&](const crow::request& req, int id) {
    // Authenticate and check user role using middleware
    std::pair<bool, std::string> user = authMiddleware(req, app);
    
    if (user.first) {
        // If the user is authenticated, proceed to update the product
        crow::response res = updateProductById(req, id, user.second);
        return res;
    } else {
        // If the user is not authenticated, return a 401 Unauthorized response
        return crow::response(401, "Unauthorized");
    }
});

    
    // Delete a product
CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::DELETE)
([&](const crow::request& req, int id) {
    // Authenticate and check user role using middleware
    std::pair<bool, std::string> user = authMiddleware(req, app);
    
    if (user.first) {
        // If the user is authenticated, proceed to delete the product
        crow::response res = deleteProductById(id, user.second);
        return res;
    } else {
        // If the user is not authenticated, return a 401 Unauthorized response
        return crow::response(401, "Unauthorized");
    }
});





    app.port(18080).multithreaded().run();

    return 0;
}
