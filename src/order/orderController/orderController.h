#ifndef ORDER_CONTROLLER_H
#define ORDER_CONTROLLER_H
#include<iostream>
#include"crow.h"

crow::response placeOrder(const crow::request &, std::string &);

crow::response getOrderDetailsById(const crow::request &req, int &order_id, std::string user_email);

crow::response getAllOrders(const crow::request &req, std::string user_email);

crow::response manageOrderStatus(const crow::request &req,int order_id, std::string user_email);

#endif