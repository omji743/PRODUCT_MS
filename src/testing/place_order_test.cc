#include<gtest/gtest.h>
#include<order/orderController/orderController.h>
#include "crow.h"

TEST(PlaceOrderTest,InvalidRequestInJson){
    std::string user_email="kunal54@gmail.com";
    crow::request req;
    req.body={};
    crow::response res =placeOrder(req,user_email);
    EXPECT_EQ(res.code,400);
    EXPECT_EQ(res.body,"Invalid JSON");
}

TEST(PlaceOrderTest, UserNotFound) {
    std::string user_email = "kunal0054@gmail.com";

    crow::request req;
    req.body = R"({
        "products": [
            {
                "product_id": 1,
                "quantity": 2
            },
            {
                "product_id": 2,
                "quantity": 1
            }
        ]
    })";

    crow::response res = placeOrder(req, user_email);

    EXPECT_EQ(res.code, 404); 
    EXPECT_EQ(res.body, "User not found");
}

TEST(PlaceOrderTest, ProductIdNotFound) {
    std::string user_email = "kunal54@gmail.com";

    crow::request req;
    req.body = R"({
        "products": [
            {
                "product_id": 101,
                "quantity": 2
            },
            {
                "product_id": 2,
                "quantity": 1
            }
        ]
    })";

    crow::response res = placeOrder(req, user_email);

    EXPECT_EQ(res.code, 404); 
    EXPECT_EQ(res.body, "Product not found");
}

TEST(PlaceOrderTest, InsufficientStock) {
    std::string user_email = "kunal54@gmail.com";

    crow::request req;
    req.body = R"({
        "products": [
            {
                "product_id": 1,
                "quantity": 201
            },
            {
                "product_id": 2,
                "quantity": 1
            }
        ]
    })";

    crow::response res = placeOrder(req, user_email);

    EXPECT_EQ(res.code, 400);
    EXPECT_EQ(res.body, "Insufficient stock for product: 1");
}

TEST(PlaceOrderTest, OrderPlaced) {
    std::string user_email = "kunal54@gmail.com";

    crow::request req;
    req.body = R"({
        "products": [
            {
                "product_id": 1,
                "quantity": 2
            },
            {
                "product_id": 2,
                "quantity": 1
            }
        ]
    })";

    crow::response res = placeOrder(req, user_email);

    // Test assertions
    EXPECT_EQ(res.code, 201);  // Expect HTTP status 400 (Bad Request)
    EXPECT_EQ(res.body, "Order placed successfully");
}
