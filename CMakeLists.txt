# initialize project
cmake_minimum_required(VERSION 3.5)
project(AlbaLisp VERSION 1.0.0 LANGUAGES C)

# load packages
#TODO: no packages for now

# create target executable
file(GLOB SRC src/**.c)
add_executable(AlbaLisp ${SRC})

# include directories
target_include_directories(AlbaLisp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# compiler properties
target_compile_options(AlbaLisp PRIVATE -Wall)
target_compile_options(AlbaLisp PRIVATE -Werror)

# export compilation database for YCM
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# link libraries
target_link_libraries(AlbaLisp
    PRIVATE
        edit
)

# copy resources from resource directories into build directory
set(source "${CMAKE_SOURCE_DIR}/assets")
set(destination "${CMAKE_CURRENT_BINARY_DIR}/assets")
add_custom_command(
    TARGET AlbaLisp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${source} ${destination} 
    DEPENDS ${destination}
    COMMENT "Created symbolic link to assets folder at ${source} to ${destination}"
)

# copy compile commands into root directory
set(source "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json")
set(destination "${CMAKE_SOURCE_DIR}/")
add_custom_command(
    TARGET AlbaLisp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${source} ${destination}
    DEPENDS ${destination}
    COMMENT "Copied compilation database from ${source} to ${destination}"
)
