cmake_minimum_required(VERSION 3.21)

project(crowsample)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)

# Set include directories for other dependencies
set(INCLUDE_PATHS boost_1_81_0 lib/crow/include)

# MySQL include and library paths
set(MYSQL_INCLUDE_DIR "/usr/include/cppconn")
set(MYSQL_LIB_DIR "/usr/lib/x86_64-linux-gnu")

# bcrypt include and library paths
set(BCRYPT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/bcrypt)
set(BCRYPT_LIB_DIR ${CMAKE_SOURCE_DIR}/lib/bcrypt)

# JWT include path (header-only)
set(JWT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/jwt-cpp/include)

# Add include directories
include_directories(${MYSQL_INCLUDE_DIR} ${BCRYPT_INCLUDE_DIR} ${JWT_INCLUDE_DIR} ${INCLUDE_PATHS} ${CMAKE_SOURCE_DIR}/src)

# Add executable target
add_executable(crowsample 
    src/main.cpp
    src/auth/auth.cpp  # Source file for user authentication
    src/db/db_connection.cpp  # Source file for database connection
)

# Link against MySQL, bcrypt
target_link_libraries(crowsample
    ${MYSQL_LIB_DIR}/libmysqlcppconn.so 
    ${BCRYPT_LIB_DIR}/bcrypt.a 
)

# Find and link OpenSSL libraries
find_package(OpenSSL REQUIRED)
target_link_libraries(crowsample OpenSSL::SSL OpenSSL::Crypto)
