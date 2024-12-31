#include "crow.h"
#include "product/Product.h"
#include "product/ProductDatabase.h"
#include "db/db_connection.h"
#include "helper/isAdmin.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

ProductDatabase product_db;

crow::json::wvalue getAllProducts()
{
    std::vector<crow::json::wvalue> vector_of_wvalue;

    auto products = product_db.get_all_products();
    crow::json::wvalue resp;

    for (const auto &product : products)
    {
        crow::json::wvalue product_json;
        product_json["id"] = product.id;
        product_json["product_name"] = product.productName;
        product_json["price"] = product.productPrice;
        product_json["stock"] = product.productStock;
        product_json["category"] = product.productCategory;
        product_json["created_at"] = product.created_at;
        product_json["updated_at"] = product.updated_at;
        vector_of_wvalue.push_back(product_json);
    }

    resp = crow::json::wvalue::list(vector_of_wvalue);
    return resp;
}

crow::response addProduct(const crow::request &req, std::string user_email)
{
    // Initialize file logger
    auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
    spdlog::set_default_logger(file_logger);

    // Check if the user is an admin
    if (!isAdmin(user_email))
    {
        spdlog::warn("Access denied for user: {}. Only admins can add products", user_email);
        return crow::response(403, "Access denied: Only admins can add products");
    }

    // Connect to the database
    sql::Connection *con = getDbConnection();
    if (!con)
    {
        spdlog::error("Failed to connect to the database");
        return crow::response(500, "Failed to connect to the database");
    }

    try
    {
        // Parse the incoming product data
        auto json_data = crow::json::load(req.body);
        if (!json_data)
        {
            spdlog::error("Invalid JSON in request body");
            delete con;
            return crow::response(400, "Invalid JSON");
        }

        // Extract product information from JSON
        std::string name = json_data["product_name"].s(); 
        double price = json_data["price"].d();
        int stock = json_data["stock"].i();
        std::string category = json_data["category"].s();

        // Log the extracted product details
        spdlog::info("Adding product: name={}, price={}, stock={}, category={}", name, price, stock, category);

        product_db.add_product(Product(name, price, stock, category));

        delete con;

        spdlog::info("Product added successfully: {}", name);
        return crow::response(201, "Product added successfully");
    }
    catch (sql::SQLException &e)
    {
        // Log and return an error response
        spdlog::error("Database error while adding product: {}", e.what());
        delete con;
        return crow::response(500, "Database error: " + std::string(e.what()));
    }
}

crow::response getProductById(const crow::request &req, int id)
{
    auto product = product_db.get_product_by_id(id);
    if (product.id == 0)
    {
        return crow::response(404, "Product not found");
    }
    crow::json::wvalue product_json;
    product_json["id"] = product.id;
    product_json["product_name"] = product.productName;
    product_json["price"] = product.productPrice;
    product_json["stock"] = product.productStock;
    product_json["category"] = product.productCategory;
    product_json["created_at"] = product.created_at;
    product_json["updated_at"] = product.updated_at;
    return crow::response(200, product_json);
}

crow::response updateProductById(const crow::request& req, int id, const std::string& user_email) {


    auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
    spdlog::set_default_logger(file_logger);
    if (!isAdmin(user_email)) {
        spdlog::warn("Access denied for user: {}. Only admins can update products", user_email);
        return crow::response(403, "Access denied: Only admins can update products");
    }

    // Parse and validate the incoming product data
    auto json_data = crow::json::load(req.body);
    if (!json_data) {
        spdlog::error("Invalid JSON in request body for product ID: {}", id);
        return crow::response(400, "Invalid JSON");
    }

    // Extract product information from JSON
    std::string name = json_data["product_name"].s();
    auto price = json_data["price"].d();
    auto stock = json_data["stock"].i();
    std::string category = json_data["category"].s();

    spdlog::info("Updating product with ID: {}. New data - name={}, price={}, stock={}, category={}", id,name, price, stock, category);

    // Update the product in the database
    product_db.update_product(Product(id, name, price, stock, category, "", ""));

    spdlog::info("Product with ID: {} successfully updated", id);

    return crow::response(200, "Product updated");
}

crow::response deleteProductById(int id, const std::string& user_email) {

    auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
    spdlog::set_default_logger(file_logger);
    if (!isAdmin(user_email)) {
        spdlog::warn("Access denied for user: {}. Only admins can delete products", user_email);
        return crow::response(403, "Access denied: Only admins can delete products");
    }

    spdlog::info("Deleting product with ID: {}", id);

    // Delete the product from the database
    product_db.delete_product(id);

    spdlog::info("Product with ID: {} successfully deleted", id);

    return crow::response(200, "Product deleted");
}

crow::response getAvailableStock(const crow::request &req, std::string user_email)
{
    crow::json::wvalue responseJson;

    // Assuming user_email is passed in URL parameters for authorization
    if (!isAdmin(user_email))
    {
        responseJson["error"] = "Permission denied: Only admins can view available stock.";
        return crow::response(403, responseJson); // Return 403 Forbidden if the user is not an admin
    }

    try
    {
        sql::Connection *con = getDbConnection(); // Get a connection to the database
        sql::PreparedStatement *stmt = con->prepareStatement(
            "SELECT id, product_name, stock FROM products;");
        sql::ResultSet *res = stmt->executeQuery();

        // Create a JSON array to hold product stock data
        std::vector<crow::json::wvalue> productStockArray;

        while (res->next())
        {
            int product_id = res->getInt("id");
            std::string product_name = res->getString("product_name");
            int stock = res->getInt("stock");

            // Create a JSON object for each product with stock information
            crow::json::wvalue productJson;
            productJson["product_id"] = product_id;
            productJson["product_name"] = product_name;
            productJson["stock"] = stock;

            // Add the product JSON object to the array
            productStockArray.push_back(productJson);
        }

        // Wrap the list of products and stock in the final JSON response
        responseJson = crow::json::wvalue::list(productStockArray);
        return crow::response(responseJson);
    }
    catch (const sql::SQLException &e)
    {
        std::cerr << "Error fetching product stock: " << e.what() << std::endl;
        responseJson["error"] = "Error fetching product stock from the database.";
        return crow::response(500, responseJson); // Return 500 error in case of a database issue
    }
    catch (const std::exception &e)
    {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        responseJson["error"] = "An unexpected error occurred.";
        return crow::response(500, responseJson); // Return 500 error for unexpected issues
    }
}
