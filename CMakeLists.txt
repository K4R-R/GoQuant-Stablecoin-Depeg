cmake_minimum_required(VERSION 3.15)
project(StablecoinTracker LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

find_package(Crow CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(CURL REQUIRED)

add_executable(StablecoinTracker 
    src/main.cpp 
    src/PriceMonitor.cpp
    src/RiskScorer.cpp
    src/AlertManager.cpp
    src/ChainMonitor.cpp
    src/HistoryManager.cpp
    src/server.cpp
)

target_include_directories(StablecoinTracker PRIVATE include)

target_link_libraries(StablecoinTracker 
    PRIVATE 
    Crow::Crow 
    nlohmann_json::nlohmann_json 
    CURL::libcurl
)
