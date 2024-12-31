#include <crow.h>
#include <jwt-cpp/jwt.h>
#include <iostream>
#include "crow/middlewares/cookie_parser.h"

std::pair<bool,std::string> authMiddleware(const crow::request& req, crow::App<crow::CookieParser>& app)
{
    auto& ctx = app.get_context<crow::CookieParser>(req);
    auto token = ctx.get_cookie("token");

    if (token.empty()) {
        std::cout << "Token is missing!" << std::endl;
        return std::make_pair(false,"Token is missing!");
    }

    // JWT decoding and validation logic here
    try {
        std::string secret = "your_jwt_secret";
        auto decoded_token = jwt::decode(token);

        // Verify the token
        jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{secret})
            .with_issuer("auth_service")
            .verify(decoded_token);

        // Extract user information from the decoded token
        auto userIdClaim = decoded_token.get_payload_claim("userId");
        auto emailClaim = decoded_token.get_subject();

        std::string userId = userIdClaim.as_string();

        return std::make_pair(true,emailClaim);


    } catch (const jwt::error::token_verification_exception& e) {
        std::cout << "Token verification failed: " << e.what() << std::endl;
        return std::make_pair(false,"Token verification failed: ");
    }
}
