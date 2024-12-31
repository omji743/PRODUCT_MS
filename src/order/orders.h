#ifndef Orders_H
#define Orders_H
#include<iostream>
#include"crow.h"
#include <mysql_connection.h>

class Order{
    public:
    int order_id;
    int user_id;
    std::string order_date;
    double total_price;
    std::string status;

    Order(int order_id, int user_id, std::string order_date, double total_price, std::string status):
    order_id(order_id),user_id(user_id),order_date(order_date), total_price(total_price),status(status){};

    static crow::response manageStatus(sql::Connection* con, int order_id,std::string updateStatus);

};


#endif