# CONFIGURATION
cmake_minimum_required(VERSION 3.24...4.1.2)
project(PHI
    VERSION 0.1.1
    LANGUAGES CXX
)

set(EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
#

# GLOBAL VARIABLES
set(PHI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/phi/)
set(PHID_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/phid/)
set(SHARED_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/shared/)

file(GLOB_RECURSE PHI_SRC CONFIGURE_DEPENDS ${PHI_SRC_DIR}/*.cpp)
file(GLOB_RECURSE PHID_SRC CONFIGURE_DEPENDS ${PHID_SRC_DIR}/*.cpp)
file(GLOB_RECURSE SHARED_SRC CONFIGURE_DEPENDS ${SHARED_SRC_DIR}/*.cpp)
#

# PHI
add_executable(phi ${PHI_SRC} ${SHARED_SRC})

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_include_directories(phi PUBLIC /opt/homebrew/include)
    target_link_directories(phi PUBLIC /opt/homebrew/lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_include_directories(phi PUBLIC /home/linuxbrew/.linuxbrew/include)
    target_link_directories(phi PUBLIC /home/linuxbrew/.linuxbrew/lib)
endif()

target_include_directories(
    phi PUBLIC
    /usr/local/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)
target_link_directories(
    phi PUBLIC
    /usr/local/lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/
)
target_link_libraries(phi fmt)

target_compile_options(phi PRIVATE
    -Wall
    -Wextra
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-return-type
    -Wno-comment
    $<$<CONFIG:Release>:-O1>
    $<$<CONFIG:Debug>:-g>
)
#

# PHID
add_executable(phid ${PHID_SRC} ${SHARED_SRC})

if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    target_include_directories(phid PUBLIC /opt/homebrew/include)
    target_link_directories(phid PUBLIC /opt/homebrew/lib)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_include_directories(phid PUBLIC /home/linuxbrew/.linuxbrew/include)
    target_link_directories(phid PUBLIC /home/linuxbrew/.linuxbrew/lib)
endif()

target_include_directories(
    phid PUBLIC
    /usr/local/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
)

target_link_directories(
    phid PUBLIC
    /usr/local/lib/
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/
)
target_link_libraries(phid zlibcomplete z cryptopp sodium SQLiteCPP sqlite3 fmt)

target_compile_options(phid PRIVATE
    -Wall
    -Wextra
    -Wno-unused-function
    -Wno-unused-parameter
    -Wno-return-type
    -Wno-comment
    $<$<CONFIG:Release>:-O2>
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