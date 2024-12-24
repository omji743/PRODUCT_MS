# cmake_minimum_required(VERSION 3.30)
# project(ProductCRUD)

# set(CMAKE_CXX_STANDARD 11)

# set(MYSQL_INCLUDE_DIR "/usr/include/cppconn")
# set(MYSQL_LIB_DIR "/usr/lib/x86_64-linux-gnu")

# include_directories(${MYSQL_INCLUDE_DIR})

# find_package(Crow CONFIG REQUIRED)

# # find_package(MySQLConnectorCpp REQUIRED)

# add_executable(ProductCRUD src/main.cpp src/Product.h src/ProductDatabase.h src/ProductDatabase.cpp)

# # target_link_libraries(ProductCRUD )
# target_link_libraries(ProductCRUD ${MYSQL_LIB_DIR}/libmysqlcppconn.so)
# target_link_libraries(ProductCRUD PRIVATE Crow::Crow)

cmake_minimum_required(VERSION 3.10)
project(ProductCRUD)

set(CMAKE_CXX_STANDARD 11)

set(MYSQL_INCLUDE_DIR "/usr/include/cppconn")
set(MYSQL_LIB_DIR "/usr/lib/x86_64-linux-gnu")

include_directories(${MYSQL_INCLUDE_DIR})

find_package(Crow CONFIG REQUIRED)

add_executable(ProductCRUD src/main.cpp src/Product.h src/ProductDatabase.h src/ProductDatabase.cpp)

# Use the keyword signature
target_link_libraries(ProductCRUD 
    PRIVATE 
    ${MYSQL_LIB_DIR}/libmysqlcppconn.so 
    Crow::Crow
)
