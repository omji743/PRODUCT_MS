#include "crow.h"
#include "product/Product.h"
#include "product/ProductDatabase.h"
#include <vector>

int main() {
    crow::SimpleApp app;
    ProductDatabase product_db;
    // CROW_ROUTE(app, "/read")
// ([&](const crow::request& req) {
    //     auto& ctx = app.get_context<crow::CookieParser>(req);
    //     // Read cookies with get_cookie
    //     auto value = ctx.get_cookie("my_cookie");
    //     return "value: " + value;
    // });

    // Create a product

    CROW_ROUTE(app, "/products").methods(crow::HTTPMethod::POST)([&product_db](const crow::request& req) {
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
        auto name = json_data["product_name"].s();
        auto price = json_data["price"].d();
        auto stock = json_data["stock"].i();
        auto category = json_data["category"].s();

        product_db.add_product(Product(name, price, stock, category));
        return crow::response(201, "Product added");
    });

    // Get all products
    CROW_ROUTE(app, "/products")([&product_db]() {
        crow::json::wvalue resp;
        std::vector<crow::json::wvalue> vector_of_wvalue;
        auto products = product_db.get_all_products();

        for (const auto& product : products) {
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
        return crow::response(200, resp);
    });

    // Get a single product by ID
    CROW_ROUTE(app, "/products/<int>")([&product_db](const crow::request& req, int id) {
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
    });

    // Update a product
    CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::PUT)([&product_db](const crow::request& req, int id) {
        auto json_data = crow::json::load(req.body);
        if (!json_data) {
            return crow::response(400, "Invalid JSON");
        }
         auto name = json_data["product_name"].s();
        auto price = json_data["price"].d();
        auto stock = json_data["stock"].i();
        auto category = json_data["category"].s();
        product_db.update_product(Product(id, name, price, stock, category, "", ""));
        return crow::response(200, "Product updated");
    });
    
    // Delete a product
    CROW_ROUTE(app, "/products/<int>").methods(crow::HTTPMethod::DELETE)([&product_db](const crow::request& req, int id) {
        product_db.delete_product(id);
        return crow::response(200, "Product deleted");
    });

    app.port(18080).multithreaded().run();
}
