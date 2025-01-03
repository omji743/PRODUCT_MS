#include "orderController.h"
#include "db/db_connection.h"
#include "../orders.h"
#include "../orderDetails.h"
#include <cppconn/prepared_statement.h>
#include"crow.h"
#include"helper/isAdmin.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

double getProductPrice(sql::Connection* con, int product_id) {
    sql::PreparedStatement *stmt = con->prepareStatement("SELECT price FROM products WHERE id = ?");
    stmt->setInt(1, product_id);
    sql::ResultSet *res = stmt->executeQuery();

    if (res->next()) {
        return res->getDouble(1);
    }
    std::cerr << "Product with id " << product_id << " not found." << std::endl;
    return 0.0; 
}

crow::response placeOrder(const crow::request &req, std::string& user_email)
{
    try
    {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);

        spdlog::info("Received request to place order for user: {}", user_email);

        sql::Connection *con = getDbConnection();
        auto json_data = crow::json::load(req.body);
        crow::response response;

        if (!json_data)
        {
            spdlog::error("Invalid JSON in request body");
            delete con;
            return crow::response(400, "Invalid JSON");
        }

        auto products = json_data["products"];

        spdlog::info("Fetching user data from database for email: {}", user_email);
        sql::PreparedStatement *stmt = con->prepareStatement("SELECT user_id FROM users WHERE email = ?");
        stmt->setString(1, user_email);
        sql::ResultSet *res = stmt->executeQuery();

        // If the user is not found
        if (!res->next()) {
            spdlog::warn("User with email {} not found", user_email);
            delete con;
            return crow::response(404, "User not found");
        }

        int user_id = res->getInt("user_id");
        spdlog::info("User found with ID: {}", user_id);

        // Calculate total price and check stock availability
        double total_price = 0;
        for (const auto &product : products)
        {
            int id = product["product_id"].i();
            int quantity = product["quantity"].i();

            spdlog::info("Checking stock for product ID: {} with requested quantity: {}", id, quantity);

            // Get the price of the product at order time
            double price_at_order_time = getProductPrice(con, id);

            // Add the product price to the total price
            total_price += price_at_order_time * quantity;

            // Check if there is enough stock for the product
            stmt = con->prepareStatement("SELECT stock FROM products WHERE id = ?");
            stmt->setInt(1, id);
            res = stmt->executeQuery();

            if (!res->next()) {
                spdlog::warn("Product ID {} not found", id);
                delete con;
                return crow::response(404, "Product not found");
            }

            int available_stock = res->getInt("stock");

            if (available_stock < quantity) {
                spdlog::warn("Insufficient stock for product ID {}. Requested: {}, Available: {}", id, quantity, available_stock);
                delete con;
                return crow::response(400, "Insufficient stock for product: " + std::to_string(id));
            }
        }

        // Insert the order into the orders table and get the order_id
        spdlog::info("Inserting order into the database for user ID: {} with total price: {}", user_id, total_price);
        stmt = con->prepareStatement("INSERT INTO Orders (user_id, total_price, status) VALUES (?, ?, ?)");
        stmt->setInt(1, user_id);
        stmt->setDouble(2, total_price);
        stmt->setString(3, "Placed!");
        stmt->executeUpdate();

        // Retrieve the generated order_id using LAST_INSERT_ID()
        stmt = con->prepareStatement("SELECT LAST_INSERT_ID()");
        res = stmt->executeQuery();

        int order_id = -1;
        if (res->next())
        {
            order_id = res->getInt(1); 
        }

        // If order_id was not generated, return an error
        if (order_id == -1)
        {
            spdlog::error("Failed to generate order_id after inserting order for user ID: {}", user_id);
            delete con;
            return crow::response(500, "Failed to create order");
        }

        spdlog::info("Order placed successfully with order ID: {}", order_id);

        // Insert each product into the order_details table and update stock
        for (const auto &product : products)
        {
            int product_id = product["product_id"].i();
            int quantity = product["quantity"].i();
            double price_at_order_time = getProductPrice(con, product_id);  

            spdlog::info("Inserting product details for order ID: {}. Product ID: {}, Quantity: {}, Price: {}", order_id, product_id, quantity, price_at_order_time);

            // Create an OrderDetails object for each product
            OrderDetails orderDetail(order_id, product_id, quantity, price_at_order_time);

            // Insert the order detail into the database
            if (OrderDetails::insertOrderDetail(con, orderDetail) != 0)
            {
                spdlog::error("Failed to insert product details into order details table for order ID: {}", order_id);
                delete con;
                return crow::response(500, "Failed to add product to the order");
            }

            // Update the stock for the product after the order is placed
            stmt = con->prepareStatement("UPDATE products SET stock = stock - ? WHERE id = ?");
            stmt->setInt(1, quantity);  
            stmt->setInt(2, product_id);
            stmt->executeUpdate();
        }

        // All products have been added to the order details table successfully and stock updated
        spdlog::info("All products added to order details. Stock updated.");
        delete con;
        return crow::response(201, "Order placed successfully");

    }
    catch (sql::SQLException &e)
    {
        spdlog::error("SQL Exception: {}", e.what());
        return crow::response(500, "Database error: Unable to place order");
    }
    catch (const std::exception &e)
    {
        spdlog::error("General Exception: {}", e.what());
        return crow::response(500, "Internal server error");
    }
}

crow::response getOrderDetailsById(const crow::request& req, int& order_id, std::string user_email) {
    try {
        // Check if the user is an admin
        if (!isAdmin(user_email)) {
            crow::json::wvalue errorJson;
            errorJson["error"] = "Permission denied: Only admins can access this data.";
            return crow::response(403, errorJson); // 403 Forbidden
        }

        sql::Connection* con = getDbConnection();
        // Get the order details as a JSON object from the OrderDetails class
        crow::json::wvalue orderDetailsJson = OrderDetails::getOrderDetailsByOrderId(con, order_id);

        // Check if order details are empty, and return a 404 if no data is found
        if (orderDetailsJson.size()==1) {
            crow::json::wvalue errorJson;
            errorJson["error"] = "Order not found for the given order ID.";
            return crow::response(404, errorJson); // 404 Not Found
        }

        // Return the response with JSON payload
        return crow::response(200, orderDetailsJson);
    }
    catch (const sql::SQLException& e) {
        std::cerr << "Database error: " << e.what() << '\n';
        crow::json::wvalue errorJson;
        errorJson["error"] = "An error occurred while fetching order details.";
        return crow::response(500, errorJson);  // Return a 500 HTTP error code with the error message
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        crow::json::wvalue errorJson;
        errorJson["error"] = "An unexpected error occurred.";
        return crow::response(500, errorJson);  // Return a 500 HTTP error code with the error message
    }
}

crow::response getAllOrders(const crow::request &req, std::string user_email) {
    crow::json::wvalue response;
    try {
        // Check if the user is an admin
        if (!isAdmin(user_email)) {
            crow::json::wvalue errorJson;
            errorJson["error"] = "Permission denied: Only admins can access this data.";
            return crow::response(403, errorJson); // 403 Forbidden
        }

        sql::Connection* conn = getDbConnection();
        sql::PreparedStatement *stmt = conn->prepareStatement("SELECT * FROM Orders;");
        sql::ResultSet* res = stmt->executeQuery();
        std::vector<crow::json::wvalue> orders;

        while (res->next()) {
            int order_id = res->getInt("order_id");
            int user_id = res->getInt("user_id");
            int total_price = res->getDouble("total_price");
            auto order_date = res->getString("order_date");
            std::string status = res->getString("status");

            crow::json::wvalue order;
            order["order_id"] = order_id;
            order["user_id"] = user_id;
            order["total_price"] = total_price;
            order["order_date"] = order_date;
            order["status"] = status;

            // Add the individual order detail to the array
            orders.push_back(order);
        }

        response = crow::json::wvalue::list(orders);
        return crow::response(response);

    }
    catch (const sql::SQLException& e) {
        std::cerr << e.what() << '\n';
        response["error"] = "Error while fetching the data.";
        return crow::response(500, response);  // Return a 500 error with message
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << '\n';
        response["error"] = "An unexpected error occurred.";
        return crow::response(500, response);  // Return a 500 error with message
    }
}

crow::response manageOrderStatus(const crow::request &req, int order_id, std::string user_email) {
    try {

        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);
        spdlog::info("Received request to update order status for order ID: {} by user: {}", order_id, user_email);

        if (!isAdmin(user_email)) {
            spdlog::warn("Permission denied for user: {}. Only admins can update order status.", user_email);
            crow::json::wvalue errorJson;
            errorJson["error"] = "Permission denied: Only admins can update order status.";
            return crow::response(403, errorJson);
            }


        sql::Connection* con = getDbConnection();
        auto json_data = crow::json::load(req.body);

        if (!json_data)
        {
            spdlog::error("Invalid JSON received in the request body for order ID: {}", order_id);
            crow::json::wvalue errorJson;
            errorJson["error"] = "Invalid JSON";
            return crow::response(400, errorJson);
        }
        if(!json_data.has("updated_status")){
            spdlog::error("Missing fields in the request body for order ID: {}", order_id);
            crow::json::wvalue errorJson;
            errorJson["error"] = "Missing fields";
            return crow::response(400, errorJson);
        }

        std::string updated_status = json_data["updated_status"].s();

        spdlog::info("Updating order ID: {} with new status: {}", order_id, updated_status);

        // Call to the Order class to manage the status update
        crow::response res = Order::manageStatus(con, order_id, updated_status);

        if (res.code == 200) {
            spdlog::info("Successfully updated status for order ID: {}", order_id);
        } else {
            spdlog::warn("Failed to update status for order ID: {}. Response code: {}", order_id, res.code);
        }

        return res;
    }
    catch (const sql::SQLException& e) {
        spdlog::error("SQL Exception: {} while updating status for order ID: {}", e.what(), order_id);
        crow::json::wvalue errorJson;
        errorJson["error"] = "Database error while updating the status.";
        return crow::response(500, errorJson);
    }
    catch (const std::exception& e) {
        spdlog::error("Exception: {} while updating status for order ID: {}", e.what(), order_id);
        crow::json::wvalue errorJson;
        errorJson["error"] = "An unexpected error occurred.";
        return crow::response(500, errorJson);
    }
}

crow::response ordersReport(const crow::request &req, std::string user_email) {
    try {
        // Set up logging
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);
        spdlog::info("Admin user '{}' is requesting the order report", user_email);

        // Check if the user is an Admin
        if (!isAdmin(user_email)) {
            spdlog::warn("Permission denied: User '{}' is not an admin", user_email);
            crow::json::wvalue errorJson;
            errorJson["error"] = "Permission denied: Only admins can view order report.";
            return crow::response(403, errorJson);
        }

        // Establish database connection
        spdlog::info("Establishing database connection...");
        sql::Connection *conn = getDbConnection();
        spdlog::info("Database connection established");

        sql::PreparedStatement *stmt = conn->prepareStatement("SELECT * FROM Orders;");
        sql::ResultSet *res = stmt->executeQuery();
        spdlog::info("Executed query: SELECT * FROM Orders");

        std::vector<crow::json::wvalue> orders;

        // Iterate over each order
        while (res->next()) {
            int order_id = res->getInt("order_id");
            int user_id = res->getInt("user_id");
            double total_price = res->getDouble("total_price");
            std::string order_date = res->getString("order_date");
            std::string status = res->getString("status");

            // Fetch user information for the given user_id
            spdlog::info("Fetching user details for user_id={}", user_id);
            sql::PreparedStatement *stmt_user = conn->prepareStatement("SELECT username, email FROM users WHERE user_id=?");
            stmt_user->setInt(1, user_id);
            sql::ResultSet *res_user = stmt_user->executeQuery();

            if (!res_user->next()) {
                spdlog::warn("User not found for user_id={}", user_id);
                crow::json::wvalue errorJson;
                errorJson["error"] = "User not found for user_id: " + std::to_string(user_id);
                return crow::response(404, errorJson);
            }

            std::string user_email = res_user->getString("email");
            std::string username = res_user->getString("username");
            spdlog::info("User details found: {} ({})", username, user_email);

            // Fetch order details for the given order_id
            spdlog::info("Fetching order details for order_id={}", order_id);
            sql::PreparedStatement *stmt_details = conn->prepareStatement(
                "SELECT orderDetails.product_id, orderDetails.quantity, orderDetails.price_at_order_time "
                "FROM orderDetails WHERE orderDetails.order_id=?;"
            );
            stmt_details->setInt(1, order_id);
            sql::ResultSet *res_details = stmt_details->executeQuery();

            crow::json::wvalue products = crow::json::wvalue::list(); // Create an empty JSON array for products

            // Collect product details for this order
            int product_idx = 0;
            while (res_details->next()) {
                int product_id = res_details->getInt("product_id");
                int quantity = res_details->getInt("quantity");
                double price_at_order_time = res_details->getDouble("price_at_order_time");

                crow::json::wvalue product;
                product["product_id"] = product_id;
                product["quantity"] = quantity;
                product["price_at_order_time"] = price_at_order_time;

                // Add the product to the products array by index
                products[product_idx++] = std::move(product);
            }

            // Construct the order report
            crow::json::wvalue order_report;
            order_report["order_id"] = order_id;
            order_report["user_id"] = user_id;
            order_report["username"] = username;
            order_report["email"] = user_email;
            order_report["total_price"] = total_price;
            order_report["order_date"] = order_date;
            order_report["status"] = status;
            order_report["products"] = std::move(products);

            // Move the order report into the orders vector
            orders.push_back(std::move(order_report));
        }

        spdlog::info("Generated order report with {} orders", orders.size());

        // Return the JSON response containing the orders report
        crow::json::wvalue response;
        response["orders"] = std::move(orders);
        return crow::response(response);

    } catch (const std::exception &e) {
        // Log error message and return 500 internal server error
        spdlog::error("Error occurred while generating the order report: {}", e.what());
        crow::json::wvalue errorJson;
        errorJson["error"] = "Internal server error: " + std::string(e.what());
        return crow::response(500, errorJson);
    }
}
