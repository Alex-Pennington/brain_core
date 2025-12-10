cmake_minimum_required(VERSION 3.10)
project(BrainModemCore VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Modem core sources
set(MODEM_SOURCES
    m188110a/de110a.cpp
    m188110a/eq110a.cpp
    m188110a/g110a.cpp
    m188110a/in110a.cpp
    m188110a/ptx110a.cpp
    m188110a/rxm110a.cpp
    m188110a/t110a.cpp
    m188110a/txm110a.cpp
    m188110a/v110a.cpp
)

# Server wrapper
set(SERVER_SOURCES
    src/main.cpp
)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/m188110a)

# Definitions
add_definitions(-D_USE_MATH_DEFINES)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32)
    set(PLATFORM_LIBS ws2_32)
else()
    set(PLATFORM_LIBS pthread)
endif()

# Main executable
add_executable(brain_modem_server ${SERVER_SOURCES} ${MODEM_SOURCES})
target_link_libraries(brain_modem_server ${PLATFORM_LIBS})

# Install
install(TARGETS brain_modem_server DESTINATION bin)
