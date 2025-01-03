#include <gtest/gtest.h>
#include <crow.h>
#include "order/orderController/orderController.h"

TEST(GetOrderDetailsByIdTest, AdminCanFetchOrderDetails) {
    std::string admin_email = "admin@gmail.com";
    int order_id = 2;
        crow::request req={};

    crow::response res = getOrderDetailsById(req,order_id, admin_email);
    
    EXPECT_EQ(res.code, 200);

}

TEST(GetOrderDetailsByIdTest, NonAdminCannotFetchOrderDetails) {
    std::string user_email = "user@example.com";
    int order_id = 2;

      crow::request req={};

    crow::response res = getOrderDetailsById(req,order_id, user_email);
    
    EXPECT_EQ(res.code, 403);

    crow::json::rvalue json = crow::json::load(res.body);
    EXPECT_EQ(json["error"].s(), "Permission denied: Only admins can access this data.");
}

TEST(GetOrderDetailsByIdTest, MissingOrderId) {
    std::string admin_email = "admin@gmail.com";

    int order_id_invalid = -1;
    crow::request req_invalid;
    crow::response res_invalid = getOrderDetailsById(req_invalid, order_id_invalid, admin_email);

    EXPECT_EQ(res_invalid.code, 404);

int order_id_non_existent = 1;
crow::request req_non_existent;
crow::response res_non_existent = getOrderDetailsById(req_non_existent, order_id_non_existent, admin_email);

std::cout << "Response Code: " << res_non_existent.code << std::endl;
std::cout << "Response Body: " << res_non_existent.body << std::endl;

EXPECT_EQ(res_non_existent.code, 404);
    EXPECT_EQ(res_non_existent.body, R"({"error":"Order not found for the given order ID."})");

}

