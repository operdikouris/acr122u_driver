cmake_minimum_required(VERSION 3.22)

#regarding versioning x.y.z is (x = major version no. The developments using previous versions may not be complatible with this)
#                               (y = minor version no. Indicates new features)
#                               (z = patch version no . Bug fixes)
project(Libusb_project 
        VERSION 0.1.0 
        LANGUAGES C)


#Overwrite some default already defined variables
set(CMAKE_C_STANDARD          99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

configure_file(configure.h.in ${PROJECT_SOURCE_DIR}/inc/configure.h @ONLY)

set(ACR122U acr122u)
set(EXECUTABLE_NAME exec)
message("hello world !")
message("ROJECT_SOURCE_DIR=" ${PROJECT_SOURCE_DIR})

# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib_files_output)



add_subdirectory(src)
add_subdirectory(test_app)