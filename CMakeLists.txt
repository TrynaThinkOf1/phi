# CONFIGURATION
cmake_minimum_required(VERSION 3.24...4.1.2)
project(PHI
    VERSION 0.2.2
    LANGUAGES C CXX
)

if(APPLE)
    execute_process(
        COMMAND xcrun --sdk macosx --show-sdk-path
        OUTPUT_VARIABLE MACOS_SDK_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT "${MACOS_SDK_PATH}")
    message(STATUS "macOS SDK Path: ${CMAKE_OSX_SYSROOT}")
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
#

find_package(Boost REQUIRED COMPONENTS thread log_setup log)
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)


# GLOBAL VARIABLES
set(PHI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/phi/)
set(PHID_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/phid/)

file(GLOB_RECURSE PHI_SRC CONFIGURE_DEPENDS ${PHI_SRC_DIR}/*.cpp)
file(GLOB_RECURSE PHID_SRC CONFIGURE_DEPENDS ${PHID_SRC_DIR}/*.cpp)
#

include(FetchContent)


FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)

FetchContent_Declare(cryptopp-cmake
    GIT_REPOSITORY https://github.com/abdes/cryptopp-cmake.git
    GIT_TAG        CRYPTOPP_8_9_0
)
set(CRYPTOPP_BUILD_TESTING OFF CACHE BOOL "Build tests" FORCE)
set(CRYPTOPP_INSTALL OFF CACHE BOOL "Install Crypto++" FORCE)

FetchContent_Declare(Sodium
    GIT_REPOSITORY https://github.com/robinlinden/libsodium-cmake.git
    GIT_TAG        e5b985ad0dd235d8c4307ea3a385b45e76c74c6a
)
set(SODIUM_DISABLE_TESTS ON CACHE BOOL "Disable tests")

FetchContent_Declare(SQLiteCpp
    GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
    GIT_TAG        master
)

FetchContent_MakeAvailable(ftxui cryptopp-cmake Sodium SQLiteCpp)

if(TARGET cryptopp)
    get_target_property(CRYPTOPP_INCLUDE_DIRS cryptopp INTERFACE_INCLUDE_DIRECTORIES)
    list(FILTER CRYPTOPP_INCLUDE_DIRS EXCLUDE REGEX "cryptopp-cmake-build/cryptopp")
    set_target_properties(cryptopp PROPERTIES 
        INTERFACE_INCLUDE_DIRECTORIES "${CRYPTOPP_INCLUDE_DIRS}"
    )
endif()
#

# PHI
add_executable(phi ${PHI_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks.cpp)

target_include_directories(phi
    BEFORE PRIVATE
        ${ZLIB_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CURL_INCLUDE_DIRS}
)

target_link_libraries(phi
    PRIVATE
        ZLIB::ZLIB
        sodium
        SQLiteCpp
        cryptopp
        ftxui::screen
        ftxui::dom
        ftxui::component
        ${CURL_LIBRARIES}
)

target_compile_options(phi
    PRIVATE
        -Wall
        -Wextra
        -Wno-unused-function
        -Wno-unused-parameter
        -Wno-return-type
        -Wno-comment
        $<$<CONFIG:Release>:-O1>
)
#

# PHID
add_executable(phid ${PHID_SRC} ${CMAKE_CURRENT_SOURCE_DIR}/src/Tasks.cpp)

target_include_directories(phid 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_libraries(phid 
    PRIVATE
        Boost::thread
        Boost::log_setup
        Boost::log
)

target_compile_options(phid
    PRIVATE
        -Wall
        -Wextra
        -Wno-unused-function
        -Wno-unused-parameter
        -Wno-return-type
        -Wno-comment
        $<$<CONFIG:Release>:-O1>
)
#

# POST-CREATE
add_custom_command(TARGET phid POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/_build/compile_commands.json
    COMMENT "Copying compile_commands.json to _build/"
)
#