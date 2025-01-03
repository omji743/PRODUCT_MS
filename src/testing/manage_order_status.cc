#include<gtest/gtest.h>
#include"crow.h"
#include"order/orderController/orderController.h"
#include<mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include "db/db_connection.h"

// Test Case: Test Manage Order Status function for admin user
// TEST(ManageOrderStatusTest, UpdateStatus_AdminUser) {
//     // Setup a test case with mock data
//     std::string admin_email = "admin@gmail.com"; // Ensure this is an admin email in the system
//     int order_id = 9;  // Make sure this order ID exists in the test database

//     // Create a request to update the order status
//     crow::request req;  // Make sure this request is appropriately populated
//     req.body = R"({"updated_status": "shipped"})";  // Test status update

//     // Call the function under test
//     crow::response res = manageOrderStatus(req, order_id, admin_email);

//     // Check that the response code is 200 (OK)
//     EXPECT_EQ(res.code, 200);

// }

// // Test Case: Test Manage Order Status function for non-admin user
// TEST(ManageOrderStatusTest, UpdateStatus_NonAdminUser) {
//     // Setup a non-admin test case
//     std::string non_admin_email = "user@example.com";  // Ensure this is not an admin email
//     int order_id = 1;  // Ensure this order exists

//     // Create a request to update the order status
//     crow::request req;
//     req.body = R"({"updated_status": "shipped"})";  // Test status update

//     // Call the function under test
//     crow::response res = manageOrderStatus(req, order_id, non_admin_email);

//     // Check that the response code is 403 (Forbidden)
//     EXPECT_EQ(res.code, 403);

//     // Optionally, verify the error message in the response
//     crow::json::rvalue error_json = crow::json::load(res.body);
//     ASSERT_TRUE(error_json);
//     EXPECT_EQ(error_json["error"].s(), "Permission denied: Only admins can update order status.");
// }

// Test Case: Test Manage Order Status function with invalid JSON
TEST(ManageOrderStatusTest, InvalidJson) {
    // Setup with valid admin user
    std::string admin_email = "admin@gmail.com";  // Ensure this is an admin email
    int order_id = 1;

    // Create an invalid JSON request (missing updated_status field)
    crow::request req;
    req.body = R"({"invalid_field": "value"})";  // Invalid JSON body

    // Call the function under test
    crow::response res = manageOrderStatus(req, order_id, admin_email);

    // Check that the response code is 400 (Bad Request)
    EXPECT_EQ(res.code, 400);

    // Optionally, verify the error message in the response
    crow::json::rvalue error_json = crow::json::load(res.body);
    ASSERT_TRUE(error_json);
    EXPECT_EQ(error_json["error"].s(), "Missing fields");
}



