#include<gtest/gtest.h>
#include"crow.h"
#include<product/productController/productController.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include<db/db_connection.h>

TEST(UpdateProductTest, UpdateProductByIdAsAdmin) {
    // Prepare the request body for updating product
    std::string user_email = "admin@gmail.com"; // Assume this is an admin user
    crow::request req;
    req.body = R"({
        "product_name": "Updated Product",
        "price": 150.50,
        "stock": 100,
        "category": "Electronics"
    })";

    // Product ID that exists in the database (assumed to be pre-existing)
    int product_id = 2; 

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 200);  // Expect HTTP status 200 (OK)
    EXPECT_EQ(res.body, "Product updated");  // Ensure correct response body

}

TEST(UpdateProductTest, NonAdminUpdateProductById) {
    // Prepare the request body for updating product
    std::string user_email = "user@example.com"; // Assume this is a regular user
    crow::request req;
    req.body = R"({
        "product_name": "Updated Product",
        "price": 150.50,
        "stock": 100,
        "category": "Electronics"
    })";

    // Product ID that exists in the database
    int product_id = 1;

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 403);  // Expect HTTP status 403 (Forbidden)
    EXPECT_EQ(res.body, "Access denied: Only admins can update products");  // Ensure correct response body
}

TEST(UpdateProductTest, InvalidJsonInRequestBody) {
    std::string user_email = "admin@gmail.com"; // Admin user
    crow::request req;
    req.body = R"({
        "product_name": "Updated Product",
        "price": "invalid_price",  // Invalid price type (should be a double)
        "stock": 100,
        "category": "Electronics"
    })";

    // Product ID that exists in the database
    int product_id = 1;

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 400);  // Expect HTTP status 400 (Bad Request)
    EXPECT_EQ(res.body, "Invalid JSON");  // Ensure correct response body
}

TEST(UpdateProductTest, ProductNotFound) {
    std::string user_email = "admin@gmail.com"; // Admin user
    crow::request req;
    req.body = R"({
        "product_name": "Updated Product",
        "price": 150.50,
        "stock": 100,
        "category": "Electronics"
    })";

    // Non-existent product ID
    int product_id = 9999; // Assuming this product ID doesn't exist

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 400);  // Expect HTTP status 404 (Not Found)
    EXPECT_EQ(res.body, "Invalid Product id.");  // Ensure correct response body
}

TEST(UpdateProductTest, MissingFieldsInRequestBody) {
    std::string user_email = "admin@gmail.com"; // Admin user
    crow::request req;
    req.body = R"({
        "product_name": "Updated Product",
        "price": 150.50,
        // "stock" is missing
        "category": "Electronics"
    })";

    // Product ID that exists in the database
    int product_id = 1;

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 400);  // Expect HTTP status 400 (Bad Request)
    EXPECT_EQ(res.body, "Invalid JSON");  // Ensure response indicates invalid JSON
}

TEST(UpdateProductTest, EmptyRequestBody) {
    std::string user_email = "admin@gmail.com"; // Admin user
    crow::request req;
    req.body = "";  // Empty body

    // Product ID that exists in the database
    int product_id = 1;

    // Call the update function
    crow::response res = updateProductById(req, product_id, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 400);  // Expect HTTP status 400 (Bad Request)
    EXPECT_EQ(res.body, "Invalid JSON");  // Ensure response indicates invalid JSON
}
