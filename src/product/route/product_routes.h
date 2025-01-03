#pragma once
#include "crow.h"
#include "auth/authMiddleware/authMiddleware.h"
#include "product/productController/productController.h"


void setProductRoutes(crow::App<crow::CookieParser> &app) {
    // Add product route
    CROW_ROUTE(app, "/add-product").methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return addProduct(req, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Get all products route
    CROW_ROUTE(app, "/get-products")([]() {
        return getAllProducts();
    });

    // Get a single product by ID route
    CROW_ROUTE(app, "/products/<int>")([](const crow::request& req, int id) {
        return getProductById(req, id);
    });

    // Update product route
    CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::PUT)
    ([&](const crow::request& req, int id) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return updateProductById(req, id, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Delete product route
    CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::DELETE)
    ([&](const crow::request& req, int id) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return deleteProductById(id, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Get available stock route
    CROW_ROUTE(app, "/get-stock")
    ([&](const crow::request& req) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return getAvailableStock(req, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });
}