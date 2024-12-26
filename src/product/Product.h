#ifndef PRODUCT_H
#define PRODUCT_H
#include<string>

using namespace std;

class Product{
    public:
      int id;
      string productName;
      double productPrice;
      int productStock;
      string productCategory;
      string created_at;
      string updated_at;

      //Product(int id,const string & Name, double )
    
     Product(int id, const std::string& name, double price, int stock, const std::string& category, const std::string& created_at, const std::string& updated_at)
        : id(id), productName(name), productPrice(price), productStock(stock), productCategory(category), created_at(created_at), updated_at(updated_at) {}

    Product(const std::string& name, double price, int stock, const std::string& category)
        : productName(name), productPrice(price), productStock(stock), productCategory(category) {}

};


#endif
