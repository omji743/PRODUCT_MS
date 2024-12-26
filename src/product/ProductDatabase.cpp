#include "ProductDatabase.h"

ProductDatabase::ProductDatabase() {
    driver = sql::mysql::get_mysql_driver_instance();
    con = driver->connect("tcp://127.0.0.1:3306", "root", "Omji#4321");
     con->createStatement()->execute("CREATE DATABASE IF NOT EXISTS omji;");
    con->setSchema("omji");
    con->createStatement()->execute(
        "CREATE TABLE IF NOT EXISTS products ("" id INT AUTO_INCREMENT PRIMARY KEY,""product_name VARCHAR(255) NOT NULL,"" price DECIMAL(10, 2) NOT NULL,""stock INT NOT NULL,"" category VARCHAR(255),""created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"" updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP);"
    );
}

ProductDatabase::~ProductDatabase() {
    delete con;
}

void ProductDatabase::add_product(const Product& product) {
    sql::PreparedStatement *pstmt = con->prepareStatement(
        "INSERT INTO products (product_name, price, stock, category) VALUES (?, ?, ?, ?)"
    );
    pstmt->setString(1, product.productName);
    pstmt->setDouble(2, product.productPrice);
    pstmt->setInt(3, product.productStock);
    pstmt->setString(4, product.productCategory);
    pstmt->executeUpdate();
    delete pstmt;
}

std::vector<Product> ProductDatabase::get_all_products() {
    std::vector<Product> products;
    sql::Statement *stmt = con->createStatement();
    sql::ResultSet *res = stmt->executeQuery("SELECT * FROM products");
    while (res->next()) {
        products.push_back(Product(
            res->getInt("id"),
            res->getString("product_name"),
            res->getDouble("price"),
            res->getInt("stock"),
            res->getString("category"),
            res->getString("created_at"),
            res->getString("updated_at")
        ));
    }
    delete res;
    delete stmt;
    return products;
}

Product ProductDatabase::get_product_by_id(int id) {
    sql::PreparedStatement *pstmt = con->prepareStatement("SELECT * FROM products WHERE id = ?");
    pstmt->setInt(1, id);
    sql::ResultSet *res = pstmt->executeQuery();
    Product product("", 0, 0, "");
    if (res->next()) {
        product = Product(
            res->getInt("id"),
            res->getString("product_name"),
            res->getDouble("price"),
            res->getInt("stock"),
            res->getString("category"),
            res->getString("created_at"),
            res->getString("updated_at")
        );
    }
    delete res;
    delete pstmt;
    return product;
}

void ProductDatabase::update_product(const Product& product) {
    sql::PreparedStatement *pstmt = con->prepareStatement(
        "UPDATE products SET product_name = ?, price = ?, stock = ?, category = ? WHERE id = ?"
    );
    pstmt->setString(1, product.productName);
    pstmt->setDouble(2, product.productPrice);
    pstmt->setInt(3, product.productStock);
    pstmt->setString(4, product.productCategory);
    pstmt->setInt(5, product.id);
    pstmt->executeUpdate();
    delete pstmt;
}

void ProductDatabase::delete_product(int id) {
    sql::PreparedStatement *pstmt = con->prepareStatement("DELETE FROM products WHERE id = ?");
    pstmt->setInt(1, id);
    pstmt->executeUpdate();
    delete pstmt;
}
