#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "../inc/configure.h"
#include "../inc/apdu.h"

const char get_firmware_version_command[2] = {0xd4, 0x02};
const unsigned char list_passive_targets[4] = {0xd4, 0x4a, 0x01, 0x00}; 
/*Config the CfgItem = 0x01 which is various timings
here we will set the default ATR_RES timeout (0x0b : 102.4ms) and 
increase the byte 3 timeout to the max (0x10 : 3.28sec)*/
const unsigned char rf_config_timings[6] = {0xd4, 0x32, 0x02, 0x00, 0x0b, 0x10};

void build_apdu_message_configure(unsigned char* DirectTransmit, int* len)
{
  *len = 0;
  DirectTransmit[0] = 0xff; //Class
  DirectTransmit[1] = 0X00; //Ins
  DirectTransmit[2] = 0x00; //P1
  DirectTransmit[3] = 0x00; //P2
  *len +=4;
  DirectTransmit[4] = (char) sizeof(rf_config_timings); //Lc
  *len += 1; 
  memcpy(DirectTransmit+5, rf_config_timings, 6); //Data in
  *len += 6;
}

void build_apdu_message_in_data_exchange (unsigned char* DirectTransmit, int* len, char* mif_command, 
int mif_command_size)
{
  *len = 0;
  DirectTransmit[0] = 0xff; //Class
  DirectTransmit[1] = 0X00; //Ins
  DirectTransmit[2] = 0x00; //P1
  DirectTransmit[3] = 0x00; //P2
  *len +=4;
  DirectTransmit[4] = (char) mif_command_size; //Lc
  *len += 1; 
  memcpy(DirectTransmit+5, mif_command, mif_command_size); //Data in
  *len += mif_command_size;

}

void build_apdu_message_list_passive_targets (unsigned char* DirectTransmit, int* len)
{
  *len = 0;
  DirectTransmit[0] = 0xff; //Class
  DirectTransmit[1] = 0X00; //Ins
  DirectTransmit[2] = 0x00; //P1
  DirectTransmit[3] = 0x00; //P2
  *len +=4;
  DirectTransmit[4] = (char) sizeof(list_passive_targets); //Lc
  *len += 1; 
  memcpy(DirectTransmit+5, list_passive_targets, 4); //Data in
  *len += 4;
}

void build_apdu_message_get_fw_ver (char* DirectTransmit, int* len)
{
  *len = 0;
  DirectTransmit[0] = 0xff; //Class
  DirectTransmit[1] = 0X00; //Ins
  DirectTransmit[2] = 0x00; //P1
  DirectTransmit[3] = 0x00; //P2
  *len +=4;
  DirectTransmit[4] = (char) sizeof(get_firmware_version_command); //Lc
  *len += 1; 
  memcpy(DirectTransmit+5, get_firmware_version_command, 2); //Data in
  *len += 2;
}

int decode_fw_ver_resp(unsigned char* resp, GetFwVerResp* get_fw_resp)
{
  
  /*Basic validation of the response*/
  if ((resp[0] != 0xd5) || (resp[1] != 0x03))
  {
    
    printf("Error in GetFirmwareVersion response\n");
    printf("resp[0]=%02xh resp[1]=%02xh\n", resp[0], resp[1]);
    return 1;
  }

  get_fw_resp->Ic = resp[2];
  get_fw_resp->Ver = resp[3];
  get_fw_resp->Rev = resp[4];
  get_fw_resp->Support = resp[5];

  #ifdef APDU_RESP_TRACE
  printf("APDU GetFirmwareVersion Response dump: \n");
  printf("-----------------------------\n");
  printf("Ic=%02hhxh Ver=%02hhx Rev=%02hhxh Support=%02hhxh \n", 
  get_fw_resp->Ic, get_fw_resp->Ver, get_fw_resp->Rev, get_fw_resp->Support);
  printf("(Support analysis: ISO18092 support[%s], ISO/IEC 14443 TypeB support[%s], "
   "ISO/IEC 14443 TypeB support[%s])\n", (get_fw_resp->Support & 0x4)? "yes" : "no", 
   (get_fw_resp->Support & 0x2)? "yes" : "no", (get_fw_resp->Support & 0x1)? "yes" : "no");
  printf("-----------------------------\n");
  #endif

 return 0;
}


int decode_in_list_passive_targets_resp(unsigned char* resp, InListPsvTrgtResp* Inlistresp)
{
  /*Basic validation of the response*/
  if ((resp[0] != 0xd5) || (resp[1] != 0x4b))
  {
    
    printf("Error in InListPassiveTarget response\n");
    printf("resp[0]=%02xh resp[1]=%02xh\n", resp[0], resp[1]);
    return 1;
  }

  /*Check if targets are found by checking the number of tags returned*/
  if (resp[2] == 0)
    return 2;
  
  /*Tag is found :)
  parse the message and fill the struct*/
  Inlistresp->NbTg = resp[2]; 
  Inlistresp->Tg = resp[3];
  Inlistresp->sens_res[0] = resp[4];
  Inlistresp->sens_res[1] = resp[5];
  Inlistresp->sel_res = resp[6];
  Inlistresp->nfcidlength = resp[7];
  memcpy(Inlistresp->uid, resp + 8,  (int) Inlistresp->nfcidlength);

  printf("Ticket found !\n");
  printf("ticket UID : ");
  for(int i=0; i<(int)Inlistresp->nfcidlength; i++)
  {
    printf("%02x ", (unsigned char)Inlistresp->uid[i]);
  }
  printf("\n");

  return 0;
}

int decode_in_data_exchange (unsigned char *resp)
{
  /*Basic validation of the response*/
  if ((resp[0] != 0xd5) || ((resp[1] != 0x41) && (resp[1] != 0x43)) ) //in data exchange or incomthru supported
  {
    
    printf("Error in InDataExchange response\n");
    printf("resp[0]=%02xh resp[1]=%02xh\n", resp[0], resp[1]);
    return 1;
  }

  if (resp[2] != 0x00)
  {
    printf("status: %02x\n", resp[2]);
    return 1;
  }

  for (int i=0; i<16; i++)
  {
    printf("%02x ", resp[3+i]);
  }
  printf("\n");

  return 0;

}
