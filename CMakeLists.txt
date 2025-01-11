cmake_minimum_required(VERSION 3.26 FATAL_ERROR)

set(PROJECT_NAME consoleChat_03)

project(${PROJECT_NAME})

if(UNIX)
    # Укажите путь к MySQL Connector/C++
    set(MYSQL_CONNECTOR_CPP_INCLUDE_DIR "/path/to/mysql-connector-c++/include")
    set(MYSQL_CONNECTOR_CPP_LIB_DIR "/path/to/mysql-connector-c++/lib")
endif()

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/mySQLTest.h
        src/mySQLTest.cpp
                src/serverTest.cpp
                src/serverTest.h
                src/Logger.cpp
                src/Logger.h)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/)

if(UNIX)
    # Линкуем библиотеки MySQL Connector/C++
    target_link_libraries(${PROJECT_NAME} mysqlcppconn)
    target_link_libraries(${PROJECT_NAME} mysqlclient)
endif()

if(WIN32)
    set(MYSQL_DIR "C:/Program Files/MySQL/MySQL Server 8.0")
    find_library(mysql NAMES libmysql PATHS ${MYSQL_DIR}/lib)
    target_include_directories(${PROJECT_NAME}  PRIVATE ${MYSQL_DIR}/include)
    target_link_libraries(${PROJECT_NAME}  ${mysql})
endif(WIN32)

