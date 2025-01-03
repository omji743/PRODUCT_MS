#include "auth.h"
#include <bcrypt.h>
#include <regex>
#include <jwt-cpp/jwt.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "db/db_connection.h"


crow::response userRegister(const crow::request &req)
{
    sql::Connection *con = nullptr;
    crow::json::wvalue response;

    crow::response res;

    try {
        // Setup logger
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);
        spdlog::info("Starting user registration process.");

        // Establish DB connection
        con = getDbConnection();
        spdlog::info("Connected to the database successfully.");

        sql::PreparedStatement *stmt = con->prepareStatement(
            "INSERT INTO users (username, password, email) VALUES (?, ?, ?)");

        // Parse incoming JSON request body
        crow::json::rvalue user = crow::json::load(req.body);
        if (!user) {
            spdlog::warn("Invalid JSON request received in body.");
            response["error"] = "Invalid JSON received in body.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return res;
        }

        if (!user.has("username") || !user.has("password") || !user.has("email")) {
            spdlog::warn("Missing fields in request body.");
            response["error"] = "Fields are missing.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return res;
        }

        std::string username = user["username"].s();
        std::string password = user["password"].s();
        std::string email = user["email"].s();

        if (username.empty() || password.empty() || email.empty()) {
            spdlog::warn("User registration failed. Empty fields.");
            response["error"] = "Please enter all the fields.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return res;
        }

        // Check if the user already exists by email
        sql::PreparedStatement *checkStmt = con->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE email = ?");
        checkStmt->setString(1, email);
        sql::ResultSet *resCheck = checkStmt->executeQuery();

        if (resCheck->next() && resCheck->getInt(1) > 0) {
            spdlog::warn("User registration failed. Email already exists: {}", email);
            response["error"] = "User already exists";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            delete checkStmt;
            delete resCheck;
            delete stmt;
            delete con;
            return res;
        }

        // Validate the email format using regex
        std::regex emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        if (!std::regex_match(email, emailRegex)) {
            spdlog::warn("Invalid email format provided: {}", email);
            response["error"] = "Please enter a valid email.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return res;
        }

        // Validate the password length
        if (password.length() < 8) {
            spdlog::warn("Password validation failed. Password too short.");
            response["error"] = "Password must be greater than or equal to 8 characters.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return res;
        }

        // Hash the password using bcrypt
        char hashedPassword[BCRYPT_HASHSIZE];
        char salt[BCRYPT_HASHSIZE];
        bcrypt_gensalt(12, salt);
        bcrypt_hashpw(password.c_str(), salt, hashedPassword);
        std::string hashed = std::string(hashedPassword);

        // Insert the user data into the database
        stmt->setString(1, username);
        stmt->setString(2, hashed);
        stmt->setString(3, email);

        int affectedRows = stmt->executeUpdate();

        if (affectedRows > 0) {
            spdlog::info("User registered successfully: {}", username);
            response["success"] = "User registered successfully";
            res.code = 201;  // Created
            res.body = response.dump();
        } else {
            spdlog::error("User registration failed. No rows affected.");
            response["error"] = "User registration failed";
            res.code = 500;  // Internal Server Error
            res.body = response.dump();
        }

        delete stmt;
        delete checkStmt;
        delete resCheck;
        delete con;
    } catch (sql::SQLException &e) {
        spdlog::error("SQL Error: {}", e.what());
        response["error"] = "Database error occurred";
        res.code = 500;  // Internal Server Error
        res.body = response.dump();
    } catch (const std::exception &e) {
        spdlog::error("Unexpected error: {}", e.what());
        response["error"] = "An unexpected error occurred.";
        res.code = 500;  // Internal Server Error
        res.body = response.dump();
    }

    return res;
}


std::pair<crow::response, std::string> userLogin(const crow::request &req) {
    sql::Connection *con = nullptr;
    crow::json::wvalue response;
    std::string token;

    // Initialize response object
    crow::response res;

    try {
        // Setup logger
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);
        spdlog::info("Starting user login process.");

        // Establish DB connection
        con = getDbConnection();
        spdlog::info("Connected to the database successfully.");

        // Parse incoming JSON request body
        crow::json::rvalue user = crow::json::load(req.body);

        if (!user) {
            spdlog::warn("Invalid JSON request received in body.");
            response["error"] = "Invalid JSON received in body.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return std::make_pair(std::move(res), token);
        }

        if (!user.has("email") || !user.has("password")) {
            spdlog::warn("Missing fields in request body.");
            response["error"] = "Fields are missing. Please provide both email and password.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return std::make_pair(std::move(res), token);
        }

        // Extract email and password from request JSON
        std::string email = user["email"].s();
        std::string password = user["password"].s();

        // Validate inputs
        if (email.empty() || password.empty()) {
            spdlog::warn("User login failed. Missing fields.");
            response["error"] = "Please provide both email and password.";
            res.code = 400;  // Bad Request
            res.body = response.dump();
            return std::make_pair(std::move(res), token);
        }

        // Prepare SQL statement to fetch password and user ID based on email
        sql::PreparedStatement *stmt = con->prepareStatement(
            "SELECT password, user_id FROM users WHERE email = ?");
        stmt->setString(1, email);
        sql::ResultSet *result = stmt->executeQuery();

        // Check if user exists
        if (!result->next()) {
            spdlog::warn("User login failed. Invalid email or password.");
            response["error"] = "Invalid email or password.";
            res.code = 401;  // Unauthorized
            res.body = response.dump();
            delete stmt;
            delete result;
            return std::make_pair(std::move(res), token);
        }

        std::string storedHashedPassword = result->getString("password");
        int userId = result->getInt("user_id");

        // Check if password matches
        if (bcrypt_checkpw(password.c_str(), storedHashedPassword.c_str()) != 0) {
            spdlog::warn("User login failed. Invalid email or password.");
            response["error"] = "Invalid email or password.";
            res.code = 401;  // Unauthorized
            res.body = response.dump();
        } else {
            // Generate JWT token upon successful login
            std::string secret = "your_jwt_secret";
            token = jwt::create()
                        .set_issuer("auth_service")
                        .set_subject(email)
                        .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
                        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))
                        .sign(jwt::algorithm::hs256{secret});

            spdlog::info("User logged in successfully: {}", email);
            response["success"] = "Login successful!";
            response["token"] = token;

            // Set status code to 200 OK for successful login
            res.code = 200;  // OK
            res.body = response.dump();
        }

        // Cleanup resources
        delete stmt;
        delete result;
    } catch (sql::SQLException &e) {
        spdlog::error("SQL Error: {}", e.what());
        response["error"] = "Database error occurred.";
        res.code = 500;  // Internal Server Error
        res.body = response.dump();
    } catch (const std::exception &e) {
        spdlog::error("Unexpected error: {}", e.what());
        response["error"] = "An unexpected error occurred.";
        res.code = 500;  // Internal Server Error
        res.body = response.dump();
    }

    // Cleanup the DB connection object
    delete con;

    // Return the response and token
    return std::make_pair(std::move(res), token);
}
