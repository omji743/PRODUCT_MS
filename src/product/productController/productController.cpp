#include "crow.h"
#include "product/Product.h"
#include "product/ProductDatabase.h"
#include"db/db_connection.h"
#include "helper/isAdmin.h"

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

crow::response addProduct(const crow::request &req, std::string user_email) {
    // Check if the user is an admin by calling the isAdmin function
    if (!isAdmin(user_email)) {
        return crow::response(403, "Access denied: Only admins can add products");
    }

    // Connect to the database for adding the product
    sql::Connection* con = getDbConnection();
    if (!con) {
        return crow::response(500, "Failed to connect to the database");
    }

    try {
        // Parse and validate the incoming product data
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            delete con;
            return crow::response(400, "Invalid JSON");
        }

        // Extract product information from JSON
        auto name = json_data["product_name"].s();
        auto price = json_data["price"].d();
        auto stock = json_data["stock"].i();
        auto category = json_data["category"].s();

        // Add product to the database (assuming product_db is a valid database handler)
        product_db.add_product(Product(name, price, stock, category));

        // Clean up and close the connection
        delete con;

        // Return success response
        return crow::response(201, "Product added successfully");

    } catch (sql::SQLException &e) {
        // In case of a database error, log and return an error response
        delete con;
        return crow::response(500, "Database error: " + std::string(e.what()));
    }
}

crow::response getProductById(const crow::request& req, int id){
            auto product = product_db.get_product_by_id(id);
        if (product.id == 0) {
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
    // Check if the user is an admin by calling the isAdmin function
    if (!isAdmin(user_email)) {
        return crow::response(403, "Access denied: Only admins can update products");
    }

    // Parse and validate the incoming product data
    auto json_data = crow::json::load(req.body);
    if (!json_data) {
        return crow::response(400, "Invalid JSON");
    }

    // Extract product information from JSON
    auto name = json_data["product_name"].s();
    auto price = json_data["price"].d();
    auto stock = json_data["stock"].i();
    auto category = json_data["category"].s();

    // Update the product in the database (assuming product_db.update_product() is implemented)
    product_db.update_product(Product(id, name, price, stock, category, "", ""));

    return crow::response(200, "Product updated");
}

crow::response deleteProductById(int id, const std::string& user_email) {
    // Check if the user is an admin by calling the isAdmin function
    if (!isAdmin(user_email)) {
        return crow::response(403, "Access denied: Only admins can delete products");
    }

    // Delete the product from the database (assuming product_db.delete_product() is implemented)
    product_db.delete_product(id);

    return crow::response(200, "Product deleted");
}
