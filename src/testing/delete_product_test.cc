#include<gtest/gtest.h>
#include"crow.h"
#include<product/productController/productController.h>

TEST(DeleteProductTest, UserNotAdmin) {
    std::string admin_email = "kunal54@gmail.com";
    int product_id = 14;

    crow::response res = deleteProductById(product_id, admin_email);
    
    EXPECT_EQ(res.code, 403);
    EXPECT_EQ(res.body,"Access denied: Only admins can delete products");
}

TEST(DeleteProductTest, ProductNotFound) {
    std::string admin_email = "admin@gmail.com";
    int product_id = 140;

    crow::response res = deleteProductById(product_id, admin_email);
    
    EXPECT_EQ(res.code, 404);
    EXPECT_EQ(res.body,"Product not found");
}

TEST(DeleteProductTest, ProductDeletedAsUserIsAdmin) {
    std::string admin_email = "admin@gmail.com";
    int product_id = 14;

    crow::response res = deleteProductById(product_id, admin_email);
    
    EXPECT_EQ(res.code, 200);
    EXPECT_EQ(res.body,"Product deleted");
}