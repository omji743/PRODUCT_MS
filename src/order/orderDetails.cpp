#include<iostream>
#include "orderDetails.h"
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include "crow.h"

 int OrderDetails::insertOrderDetail(sql::Connection* con, const OrderDetails& orderDetail){
        try {
            std::string query = "INSERT INTO orderDetails (order_id, product_id, quantity, price_at_order_time) "
                                "VALUES (?, ?, ?, ?)";
            sql::PreparedStatement* stmt = con->prepareStatement(query);

            stmt->setInt(1, orderDetail.order_id);
            stmt->setInt(2, orderDetail.product_id);
            stmt->setInt(3, orderDetail.quantity);
            stmt->setDouble(4, orderDetail.price_at_order_time);

            stmt->executeUpdate();
            delete stmt;
            return 0;  // Success
        } catch (sql::SQLException& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;  // Failure
        }
    }

crow::json::wvalue OrderDetails::getOrderDetailsByOrderId(sql::Connection* con, int order_id) {
    crow::json::wvalue orderDetailsJson;

    try {
        std::cout<<order_id<<std::endl;
        sql::PreparedStatement* stmt = con->prepareStatement(
            "SELECT product_id, quantity, price_at_order_time "
            "FROM orderDetails WHERE order_id = ?");
        stmt->setInt(1, order_id);
        sql::ResultSet* res = stmt->executeQuery();

        // Create a JSON array to hold the order details
        std::vector<crow::json::wvalue> orderDetailsArray;

        while (res->next()) {
            int product_id = res->getInt("product_id");
            int quantity = res->getInt("quantity");
            double price_at_order_time = res->getDouble("price_at_order_time");

            // Create a JSON object for each order detail and add it to the array
            std::cout<<product_id<<" "<<quantity<<" "<<price_at_order_time<<std::endl; 
            crow::json::wvalue orderDetailJson;
            orderDetailJson["product_id"] = product_id;
            orderDetailJson["quantity"] = quantity;
            orderDetailJson["price_at_order_time"] = price_at_order_time;

            // Add the individual order detail to the array
            orderDetailsArray.push_back(orderDetailJson);
        }
    crow::json::wvalue resp;

        // You can return the full JSON array or wrap it in a top-level JSON object
    resp = crow::json::wvalue::list(orderDetailsArray);
    return resp;

    }
    catch (sql::SQLException &e) {
        std::cerr << "Error fetching order details: " << e.what() << std::endl;
        orderDetailsJson["error"] = "Error fetching order details";
    return orderDetailsJson;

    }

}