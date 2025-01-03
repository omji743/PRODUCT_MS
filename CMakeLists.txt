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

set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/lib/spdlog/include)

# Add include directories
include_directories(${MYSQL_INCLUDE_DIR} ${BCRYPT_INCLUDE_DIR} ${JWT_INCLUDE_DIR} ${SPDLOG_INCLUDE_DIR} ${INCLUDE_PATHS} ${CMAKE_SOURCE_DIR}/src)

# Create a shared library for the core application logic
add_library(crowsample_lib 
    src/auth/auth.cpp 
    src/db/db_connection.cpp 
    src/product/ProductDatabase.cpp 
    src/auth/authMiddleware/authMiddleware.cpp
    src/product/productController/productController.cpp
    src/helper/isAdmin.cpp
    src/order/orderDetails.cpp
    src/order/orderController/orderController.cpp
    src/order/order.cpp
)

# Link against MySQL, bcrypt
target_link_libraries(crowsample_lib
    ${MYSQL_LIB_DIR}/libmysqlcppconn.so 
    ${BCRYPT_LIB_DIR}/bcrypt.a
)

# Find and link OpenSSL libraries
find_package(OpenSSL REQUIRED)
target_link_libraries(crowsample_lib OpenSSL::SSL OpenSSL::Crypto)

# Add executable target for the main application
add_executable(crowsample src/main.cpp)
target_link_libraries(crowsample crowsample_lib)

# Find and link Google Test libraries
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIBRARIES})


enable_testing()

# # Define test executable
# add_executable(
#   get_order_details_test 
#     src/testing/get_order_details_test.cc
# )

# # Link the test executable with the necessary libraries
# target_link_libraries(get_order_details_test 
#     crowsample_lib 
#     GTest::gtest_main
#     ${MYSQL_LIB_DIR}/libmysqlcppconn.so
# )

# add_executable(
#   place_order_test 
#     src/testing/place_order_test.cc
# )

# # Link the test executable with the necessary libraries
# target_link_libraries(place_order_test 
#     crowsample_lib 
#     GTest::gtest_main
#     ${MYSQL_LIB_DIR}/libmysqlcppconn.so
# )

# # Place order testing
# add_executable(
#   add_product_test 
#     src/testing/add_product_test.cc
# )

# # Link the test executable with the necessary libraries
# target_link_libraries(add_product_test 
#     crowsample_lib 
#     GTest::gtest_main
#     ${MYSQL_LIB_DIR}/libmysqlcppconn.so
# )

# Updating product
# add_executable(
#   update_product_test 
#     src/testing/update_product_test.cc
# )

# # Link the test executable with the necessary libraries
# target_link_libraries(update_product_test 
#     crowsample_lib 
#     GTest::gtest_main
#     ${MYSQL_LIB_DIR}/libmysqlcppconn.so
# )

add_executable(
    delete_product_test 
    src/testing/delete_product_test.cc
)

# Link the test executable with the necessary libraries
target_link_libraries(delete_product_test 
    crowsample_lib 
    GTest::gtest_main
    ${MYSQL_LIB_DIR}/libmysqlcppconn.so
)

# manage order status test
add_executable(
    manage_order_status 
    src/testing/manage_order_status.cc
)

# Link the test executable with the necessary libraries
target_link_libraries(manage_order_status 
    crowsample_lib 
    GTest::gtest_main
    ${MYSQL_LIB_DIR}/libmysqlcppconn.so
)

# Automatically discover and run tests
include(GoogleTest)
# gtest_discover_tests(get_order_details_test)
# gtest_discover_tests(place_order_test)
# gtest_discover_tests(add_product_test)
# gtest_discover_tests(update_product_test)
# gtest_discover_tests(delete_product_test)
gtest_discover_tests(manage_order_status)
