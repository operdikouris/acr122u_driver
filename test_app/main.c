#include <stdio.h>
#include <stdlib.h>
#include "../inc/configure.h"
#include "inttypes.h"
#include "../inc/ccid.h"
#include "../inc/apdu.h"
#include <string.h>
#include "../inc/acr122u_usb.h"
#include <unistd.h>


/************************/
/*Abbreviations****
TAMA => Philips Reader-IC, nickname for PN53x
PICC => Proximitive Incuctive coupling card. Tags based on ISO14443 standart
CCID => Integrated Circuits Cards Interface Devices/
/************************/

/*The logic behind communicating with a tag is to follow the CCID Commands referenced in 
acr122u manual (USB Intgrated Circuits Cards Interface)*/

 
unsigned char transfer_buf[256]; 
// unsigned char Get_Firmware_Version[5] = {0xff, 0x00, 0x48, 0x00, 0x00};
unsigned char DirectTransmit[260];
int  DirectTransmit_len;


int main (void)
{

  init_usb ();

  /*Power on the Icc (This will also light the red led
  in acr122u reader)*/
  power_on_reader();

  /*tranceive Get Firmware Version command */
  get_fw_ver();
  
  /*tranceive pn532 command GetFirmwareVersion*/
  pseudo_apdu_GetFirmwareVersion();

  printf("Scanning for ticktes...\n");
  poll_for_cards();

  /*Read the user memory of the card*/
  read_card_memory();

  /*increase time out*/
  set_configuration();
  // authenticate();

  return 0; 
}