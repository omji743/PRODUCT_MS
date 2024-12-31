#pragma once
#include "crow.h"
#include "order/orderController/orderController.h"
#include "crow/middlewares/cookie_parser.h"
#include "auth/authMiddleware/authMiddleware.h"



void setupOrderRoutes(crow::App<crow::CookieParser> &app) {
    // Place a new order
    CROW_ROUTE(app, "/orders").methods(crow::HTTPMethod::POST)
    ([&](const crow::request &req) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        std::string email = user.second;
        if (user.first) {
            return placeOrder(req, email);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Get a single order by ID
    CROW_ROUTE(app, "/get-order/<int>")([&](const crow::request& req, int id) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return getOrderDetailsById(req, id, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Get all orders
    CROW_ROUTE(app, "/get-orders")([&](const crow::request& req) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return getAllOrders(req, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });

    // Update order status
    CROW_ROUTE(app, "/update-status/<int>")([&](const crow::request& req, int id) {
        std::pair<bool, std::string> user = authMiddleware(req, app);
        if (user.first) {
            return manageOrderStatus(req, id, user.second);
        } else {
            return crow::response(401, "Unauthorized");
        }
    });
}