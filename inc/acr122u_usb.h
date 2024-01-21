#ifndef _ACR122U_USB_H_
#define _ACR122U_USB_H_

#include <stdio.h>


int init_usb ();
int send_to_reader (char* send_data, const size_t send_size);
int receive_from_reader (char* received_data, const size_t received_size);


#endif