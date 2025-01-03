#include<gtest/gtest.h>
#include<product/productController/productController.h>
#include "crow.h"

TEST(AddProductTest,NotAdminUser){
    std::string user_email="kunal54@gmail.com";
    crow::request req;
    req.body={};
    crow::response res =addProduct(req,user_email);
    EXPECT_EQ(res.code,403);
    EXPECT_EQ(res.body,"Access denied: Only admins can add products");
}

TEST(AddProductTest,InvalidJSON){
    std::string user_email="admin@gmail.com";
    crow::request req;
    req.body={};
    crow::response res =addProduct(req,user_email);
    EXPECT_EQ(res.code,400);
    EXPECT_EQ(res.body,"Invalid JSON");
}

TEST(AddProductTest,ProductAdded){
    std::string user_email="admin@gmail.com";
    crow::request req;
    req.body=R"({
    "product_name":"Shirt",
    "price":5201.00,
    "stock":12,
    "category":"cloths"
    })";
    crow::response res =addProduct(req,user_email);
    EXPECT_EQ(res.code,201);
    EXPECT_EQ(res.body,"Product added successfully");
}