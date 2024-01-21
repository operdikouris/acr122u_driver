add_executable(${EXECUTABLE_NAME} main.c)

target_include_directories(${EXECUTABLE_NAME} PUBLIC "/home/orfeas/acr122u_driver/inc")

target_link_libraries(${EXECUTABLE_NAME} ${ACR122U} -lusb)
