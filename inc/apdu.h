#ifndef _APDU_H_
#define _APDU_H_

/*Miscellaneous commands*/
#define Diagnose 0x00
#define GetFirmwareVersion 0x02
#define GetGeneralStatus 0x04
#define ReadRegister 0x06
#define WriteRegister 0x08
#define ReadGPIO 0x0C
#define WriteGPIO 0x0E
#define SetSerialBaudRate 0x10
#define SetParameters 0x12
#define SAMConfiguration 0x14
#define PowerDown 0x16

/*RF communication*/
typedef struct adpu_get_fw_ver_resp
{
    char Ic; 
    char Ver;
    char Rev;
    char Support; 

}GetFwVerResp;

typedef struct apdu_in_list_pass_targ_resp
{
    char NbTg; //no of tags found
    char Tg; //index of tags (1 or 2)
    char sens_res[2];
    char sel_res; 
    char nfcidlength;
    char uid[7];

}InListPsvTrgtResp;

void build_apdu_message_list_passive_targets (unsigned char* DirectTransmit, int* len);
void build_apdu_message_get_fw_ver (char *DirectTransmit, int* len);
void build_apdu_message_in_data_exchange (unsigned char* DirectTransmit, int* len, char* mif_command, 
int mif_command_size);
int decode_fw_ver_resp(unsigned char* resp, GetFwVerResp* get_fw_resp);
int decode_in_list_passive_targets_resp(unsigned char* resp, InListPsvTrgtResp* Inlistresp);
int decode_in_data_exchange (unsigned char *resp);
void build_apdu_message_configure(unsigned char* DirectTransmit, int* len);

#endif //_APDU_H_