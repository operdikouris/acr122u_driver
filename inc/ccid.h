#ifndef __CCID_H__
#define __CCID_H__

/*commands sended from PC to NFC Reader*/
#define PC_to_RDR_IccPowerOn 0x62
#define PC_to_RDR_XfrBlock   0x6F

/*commands sended from NFC Reader to PC*/

/*command RDR_to_PC_DataBlock is send as response
in one of the following requests :
“PC_to_RDR_IccPowerOn”,
“PC_to_RDR_Secure” and
“PC_to_RDR_XfrBlock” 
*/
#define RDR_to_PC_DataBlock 0x80


#define CCID_ICC_PWR_ON_SIZE 10
/*This is the fixed size part of the PC_to_RDR_XfrBlock (the variable size part is the abData)*/
#define CCID_XFRBLOCK_FIXED_SIZE 10

#define Get_Firmware_Version_size 5

/*Max pseudo apdu message length = max payload(255) + 5*/
#define PSEUDO_APDU_DIRECT_TRANSMIT_SIZE 260

extern char ccid_frame_icc_power_on[CCID_ICC_PWR_ON_SIZE]; 
extern char ccid_frame_xframe_block[PSEUDO_APDU_DIRECT_TRANSMIT_SIZE];

typedef struct ccid_frame_response
{
    char bMessageType;
    int  dwLength;
    char bSlot;
    char bSeq;
    char bStatus;   
    char bError;
    char bChainParameter;
    char abData[250];
}READER_RESP;


void int_to_char_array(char* arr, int num);
void ccid_set_power_on_frame();
int ccid_set_xfrblocck_frame(char* data, int len);
int ccid_parse_rdr_to_pc_datablock(unsigned char* resp, READER_RESP* reader_data);

int power_on_reader();
int get_fw_ver();
int pseudo_apdu_GetFirmwareVersion();
int pseudo_apdu_InListPassiveTarget();
int poll_for_cards();
int read_card_memory();
int authenticate();
int set_configuration();
#endif // __CCID_H__