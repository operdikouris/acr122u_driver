add_library(${ACR122U} STATIC ccid.c apdu.c acr122u_usb.c)
target_include_directories(${ACR122U} PUBLIC "/home/orfeas/acr122u_driver/inc")
