#ifndef ORDER_DETAILS_H
#define ORDER_DETAILS_H
#include<iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include "crow.h"
#include<vector>


class OrderDetails {
public:
    int order_id;
    int product_id;
    int quantity;
    double price_at_order_time;

    // Constructor to initialize the attributes
    OrderDetails(int order_id, int product_id, int quantity, double price_at_order_time)
        : order_id(order_id), product_id(product_id), quantity(quantity),
          price_at_order_time(price_at_order_time) {}

    // Method to insert order detail into the database
    static int insertOrderDetail(sql::Connection* con, const OrderDetails& orderDetail);

    static crow::json::wvalue getOrderDetailsByOrderId(sql::Connection* con, int order_id);


};
#endif