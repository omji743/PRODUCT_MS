#include "orders.h"
#include "crow.h"
#include <cppconn/prepared_statement.h>
#include <mysql_connection.h>

crow::response Order::manageStatus(sql::Connection* con, int order_id, std::string updateStatus) {
    crow::json::wvalue response;
    try {
        // First, check if the order exists by selecting it
        sql::PreparedStatement *stmt = con->prepareStatement("SELECT * FROM Orders WHERE order_id = ?");
        stmt->setInt(1, order_id);
        sql::ResultSet *res = stmt->executeQuery();

        // If no order is found, return a 404 response
        if (!res->next()) {
            return crow::response(404, "No order found.");
        }

        // Store the current status before updating
        std::string status = res->getString("status");
        std::cout << order_id << " " << updateStatus << std::endl;

        // Now, update the status of the order
        stmt = con->prepareStatement("UPDATE Orders SET status = ? WHERE order_id = ?");
        stmt->setString(1, updateStatus);
        stmt->setInt(2, order_id);

        // Execute the update query and check the number of affected rows
        int rowsAffected = stmt->executeUpdate();

        // If no rows were affected, it means the update didn't happen
        if (rowsAffected == 0) {
            return crow::response(400, "Error while updating the status.");
        }

        // Successfully updated, return the response with status change details
        return crow::response(200, "Status Changed from " + status + " to " + updateStatus);

    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return crow::response(500, "Internal Server Error");
    }
}
