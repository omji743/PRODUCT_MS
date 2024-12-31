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

// Function to register user
crow::json::wvalue userRegister(const crow::request &req)
{
    sql::Connection *con;
    crow::json::wvalue response;

    try {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);

        spdlog::info("Starting user registration process.");

        con=getDbConnection();

        spdlog::info("Connected to the database successfully.");

        sql::PreparedStatement *stmt = con->prepareStatement(
            "INSERT INTO users (username, password, email) VALUES (?, ?, ?)");

        crow::json::rvalue user = crow::json::load(req.body);
        std::string username = user["username"].s();
        std::string password = user["password"].s();
        std::string email = user["email"].s();

        if (username.empty() || password.empty() || email.empty())
        {
            spdlog::warn("User registration failed. Missing fields.");
            response["error"] = "Please enter all the fields.";
            return response;
        }

        // Check if the user already exists by email
        sql::PreparedStatement *checkStmt = con->prepareStatement(
            "SELECT COUNT(*) FROM users WHERE email = ?");
        checkStmt->setString(1, email);
        sql::ResultSet *res = checkStmt->executeQuery();

        if (res->next() && res->getInt(1) > 0)
        {
            spdlog::warn("User registration failed. Email already exists: {}", email);
            response["error"] = "User already exists";
            delete checkStmt;
            delete res;
            delete stmt;
            delete con;
            return response;
        }

        // Validate the email format using regex
        std::regex emailRegex(R"(^[a-zA-Z0-9_.+-]+@[a-zA-Z0-9-]+\.[a-zA-Z0-9-.]+$)");
        if (!regex_match(email, emailRegex))
        {
            spdlog::warn("Invalid email format provided: {}", email);
            response["error"] = "Please enter a valid email.";
            return response;
        }

        // Validate the password length
        if (password.length() < 8)
        {
            spdlog::warn("Password validation failed. Password too short.");
            response["error"] = "Password must be greater than or equals to 8.";
            return response;
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

        if (affectedRows > 0)
        {
            spdlog::info("User registered successfully: {}", username);
            response["success"] = "User registered successfully";
        }
        else
        {
            spdlog::error("User registration failed. No rows affected.");
            response["error"] = "User registration failed";
        }

        delete stmt;
        delete checkStmt;
        delete res;
        delete con;
    }
    catch (sql::SQLException &e)
    {

        spdlog::error("SQL Error: {}", e.what());
        response["error"] = "Database error occurred";
    }

    return response;
}


// Function to login user
std::pair<crow::json::wvalue, std::string> userLogin(const crow::request &req)
{
    sql::Connection *con;
    crow::json::wvalue response;
    std::string token;

    try
    {
        auto file_logger = spdlog::basic_logger_mt("file_logger", "/home/kunal/crowcpp/src/logging/logs.txt");
        spdlog::set_default_logger(file_logger);

        spdlog::info("Starting user login process.");

        con=getDbConnection();

        spdlog::info("Connected to the database successfully.");


        // Parsing request JSON body
        crow::json::rvalue user = crow::json::load(req.body);
        std::string email = user["email"].s();
        std::string password = user["password"].s();

        // Validate inputs
        if (email.empty() || password.empty())
        {
            spdlog::warn("User login failed. Missing fields.");
            response["error"] = "Please provide both email and password.";
            return std::make_pair(response, token);
        }

        // Prepare SQL statement
        sql::PreparedStatement *stmt = con->prepareStatement(
            "SELECT password, user_id FROM users WHERE email = ?");
        stmt->setString(1, email);
        sql::ResultSet *result = stmt->executeQuery();

        // Check if user exists
        if (!result->next())
        {
            spdlog::warn("User login failed. Invalid email or password.");
            response["error"] = "Invalid email or password.";
            delete stmt;
            delete result;
            delete con;
            return std::make_pair(response, token);
        }

        std::string storedHashedPassword = result->getString("password");
        int userId = result->getInt("user_id");

        // Check if password matches
        if (bcrypt_checkpw(password.c_str(), storedHashedPassword.c_str()) != 0)
        {
            spdlog::warn("User login failed. Invalid email or password.");
            response["error"] = "Invalid email or password.";
        }
        else
        {
            // Generate JWT token
            std::string secret = "your_jwt_secret";
            token = jwt::create()
                        .set_issuer("auth_service")
                        .set_subject(email)
                        .set_payload_claim("userId", jwt::claim(std::to_string(userId)))
                        .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours(1))
                        .sign(jwt::algorithm::hs256{secret});

            spdlog::info("User registered successfully: {}", email);
            response["success"] = "Login successful!";
            response["token"] = token;
        }

        // Clean up resources
        delete stmt;
        delete result;
        delete con;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "SQL Error: " << e.what() << std::endl;
        spdlog::error("SQL Error: {}", e.what());
        response["error"] = "Database error occurred.";
    }

    // Return the response JSON and token
    return std::make_pair(response, token);
}

