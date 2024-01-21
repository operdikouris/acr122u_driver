#include "../inc/ccid.h"
#include "../inc/configure.h"
#include "../inc/acr122u_usb.h"
#include "../inc/apdu.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*structure of power on frame (with byte size for each field) :
----------------------------------- 
bMessageType[1]| dwLength[4]| bSlot[1]| bSeq[1]| bPowerSelect[1]| abRFU[2]
---------------------------------
source : https://www.usb.org/sites/default/files/DWG_Smart-Card_CCID_Rev110.pdf*/

char dwLength [4] = {0x00, 0x00, 0x00, 0x00};
const char bSlot = {0x00}; //slot no #0
const char bSeq  = {0x00}; //sequence no #0
const char bPowerSelect = {0x00}; //Voltage applied to icc : 5 Volts
const char abRFU[2] = {0X00, 0X00};

const char bBWI = {0X00};// value used to extend block timeout
const char wLevelParameter[2] = {0x00, 0x00};
const char abData[255] = {0x00};

/*expand with the desired ccid frame*/
char ccid_frame_icc_power_on[CCID_ICC_PWR_ON_SIZE]; 
char ccid_frame_xframe_block[260];

char ccid_frame_response_datablock[260];

/*Message from ACR122U application programming interface
 "6.3 Get firmware version of the reader"*/
unsigned char Get_Firmware_Version[5] = {0xff, 0x00, 0x48, 0x00, 0x00};

void int_to_char_array(char* arr, int num)
{
    if (!arr)
        return;
    for (int i = 0; i < 4; i++) 
    {
        arr[i] = (num & 0xff);
        num >>= 8;
    }
}


/*The response to this command is type of RDR_to_PC_DataBlock and the 
data returned is the Answer to Reset(ATR) */
void ccid_set_power_on_frame()
{
    memset(ccid_frame_icc_power_on, 0, sizeof(ccid_frame_icc_power_on));
    ccid_frame_icc_power_on[0] = PC_to_RDR_IccPowerOn;
    memset(dwLength, 0, sizeof(dwLength)); //set length to zero
    memcpy(ccid_frame_icc_power_on + 1, dwLength, 4);
    ccid_frame_icc_power_on[5] = bSlot;
    ccid_frame_icc_power_on[6] = bSeq;
    ccid_frame_icc_power_on[7] = bPowerSelect;
    memcpy(ccid_frame_icc_power_on + 8, abRFU, 2);

#ifdef CCID_TRACE
    printf("CCID power on => \n");
    for (int i=0; i<10; i++)
    {
        printf("%02x ", ccid_frame_icc_power_on[i]);
    }
    printf("\n");  
#endif

#ifdef CCID_ANALYSIS_REQ
    printf("bMessageType=%02xh dwLength=%02x%02x%02x%02xh bSlot=%02xh bSeq=%02xh bPowerSelect=%02x abRFU=%02X%02X\n", 
    ccid_frame_icc_power_on[0], ccid_frame_icc_power_on[1], ccid_frame_icc_power_on[2], ccid_frame_icc_power_on[3], 
    ccid_frame_icc_power_on[4], ccid_frame_icc_power_on[5], ccid_frame_icc_power_on[6], ccid_frame_icc_power_on[7],
    ccid_frame_icc_power_on[8], ccid_frame_icc_power_on[9]);
#endif
}

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 * @return int 
 */
int ccid_set_xfrblocck_frame(char* data, int len)
{
    memset(ccid_frame_xframe_block, 0, sizeof(ccid_frame_xframe_block));
    if(!data || !len || len > 250)
        return 1;
    ccid_frame_xframe_block[0] = PC_to_RDR_XfrBlock;
    int_to_char_array(dwLength, len);
    memcpy(ccid_frame_xframe_block + 1, dwLength, 4);
    ccid_frame_xframe_block[5] = bSlot;
    ccid_frame_xframe_block[6] = bSeq;
    ccid_frame_xframe_block[7] = bBWI;
    memcpy(ccid_frame_xframe_block + 8, wLevelParameter, 2);
    memcpy(ccid_frame_xframe_block + 10, data, len);

#ifdef CCID_TRACE
    printf("CCID xfrblock => \n");
    for (int i=0; i<10 + len; i++)
    {
        printf("%02x ", ccid_frame_xframe_block[i]);
    }
    printf("\n");        
#endif
    return 0;
}

/**
 * @brief Function for parsing the RDR_to_PC_DataBlock (CCID protocol))received in the usb 
 * interface. It prints the message in analytic form in the standard output if the corresponding
 * symbols are enabled.
 * 
 * @param[in] resp
 * @param[out] reader_data structure that keeps the reader response
 * @return 0 on success
 *         1 on bad response format
 */
int ccid_parse_rdr_to_pc_datablock(unsigned char* resp, READER_RESP* reader_data)
{
    int len;
    
    #ifdef RDR_RESP_TRACE
    printf("Parsing reader's response...\n");
    #endif

    if (!resp )
        return 1;

    /*Check if the first byte is the command RDR_to_PC_DataBlock
    if not an error has occured*/
    if (resp[0]  != RDR_to_PC_DataBlock)
    {
        printf("resp[0]=%d RDR_to_PC_DataBlock=%d\n", resp[0], RDR_to_PC_DataBlock);
        printf("Bad format of reader response\n");
        return 1;
    }

    reader_data->bMessageType = resp[0] & 0xff;

    /*extract the length from dwLength*/
    len = 0;
    len = (resp[1] | (resp[2] << 8) | (resp[3] << 16) | (resp[4] << 24));

    reader_data->dwLength = len;
    reader_data->bSlot = resp[5];
    reader_data->bSeq = resp[6];
    reader_data->bStatus = resp[7];
    reader_data->bError = resp[8];
    reader_data->bChainParameter = resp[9];
    memcpy(reader_data->abData, resp+10, reader_data->dwLength);
    
#ifdef RDR_RESP_TRACE
    /*Decode status*/
    char icc_status_desc[100];
    char comm_status_desc[100];
    char bmICCStatus;
    char bmCommandStatus;

    bmICCStatus = (reader_data->bStatus & 0x3);
    bmCommandStatus = (reader_data->bStatus >> 6) & 0x3;

    /*add icc status description*/
    if (bmICCStatus == 0)
        strcpy(icc_status_desc, "An ICC is present and active (power is on and"
         "stable, RST is inactive)");
    else if (bmICCStatus == 1)
        strcpy(icc_status_desc, "An ICC is present and inactive (not activated or"
        "shut down by hardware error)");
    else if (bmICCStatus == 2)
        strcpy(icc_status_desc , "No ICC is present");
    else
         strcpy(icc_status_desc, "Unknown");
    
     
    /*add command status description*/
    if (bmCommandStatus == 0)
        strcpy(comm_status_desc, "Processed without error");
    else if (bmCommandStatus == 1)
        strcpy(comm_status_desc, "Failed (error code provided by the error register)");
    else if (bmCommandStatus == 2)
         strcpy(comm_status_desc, "Time Extension is requested");
    else 
        strcpy(comm_status_desc, "Unknown");

    printf("CCID RDR_to_PC_DataBlock dump: \n");
    printf("-----------------------------\n");
    printf("bMessageType=%02hhxh dwLength=%d bSlot=%02xh bSeq=%02xh \n", 
    reader_data->bMessageType, reader_data->dwLength, reader_data->bSlot, reader_data->bSeq); 
    printf("bStatus=%02xh\n", reader_data->bStatus);
    printf("(bmICCStatus=%02xh : %s bmCommandStatus=%02xh : %s)\n", bmICCStatus, icc_status_desc, 
    bmCommandStatus, comm_status_desc);
    printf("bError : %02hhxh bChainParameter=%02xh\n", reader_data->bError, reader_data->bChainParameter);
    //TODO: Add analysis for bError
    printf("abData(HEX) : ");
    for (int i=0; i<reader_data->dwLength; i++)
        printf("%02hhx ", reader_data->abData[i]);
    printf("\n");
    printf("abData(ASCII) : %s\n", reader_data->abData);
    printf("-----------------------------\n");
          
#endif

    return 0;
   

}

/**
 * @brief Handles the power on of the reader (acr122u). Sends the PC_to_RDR_IccPowerOn 
 * command (CCID protocol)
 * 
 * @return 0 on success,
 *         1 on usb error,
 *         2 on bad ccid response format,
 *         3 on memory allocation failure
 */
int power_on_reader()
{
    int ret;
    READER_RESP *reader; 
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }
    /*Power on the Icc (This will also light the red led
    in acr122u reader)*/
    ccid_set_power_on_frame();

    ret = send_to_reader(ccid_frame_icc_power_on, CCID_ICC_PWR_ON_SIZE);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }
    memset(transfer_buf, 0, sizeof(transfer_buf));

    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }

    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        return 2;
    }

    free(reader);

    return 0;

}
/**
 * @brief This function embeds the Get Firmware Version command (which is 
 * defined in the ACR122U Applicaton Programming Interface) in the data field of the  
 * PC_to_RDR_XfrBlock command (CCID protocol).
 * 
 * 
 * @return 0 on success, 
 *         1 on usb error,
 *         2 on bad ccid response format,
 *         3 on memory allocation failure
 */
int get_fw_ver()
{
    int ret; 
    READER_RESP *reader; 
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }

    ccid_set_xfrblocck_frame(Get_Firmware_Version, Get_Firmware_Version_size);

    ret = send_to_reader(ccid_frame_xframe_block, CCID_XFRBLOCK_FIXED_SIZE + Get_Firmware_Version_size);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }

    memset(transfer_buf, 0, sizeof(transfer_buf));

    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }

    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        return 2;
    }

    free(reader);
}

/**
 * @brief This function sends to the reader the Pseudo-APDU Command GetFirmwareVersion. In more detail 
 * it embeds insisde a PC_to_RDR_XfrBlock frame the Direct Transmit frame which embeds the pn532 command
 * GetFirmwareVersion. 
 * => [PC_to_RDR_XfrBlock[Direct Transmit[GetFirmwareVersion]]]
 * (Direct Transmit defined in ACR122U Application Programming Interface section 6.0)
 * (GetFirmwareVersion defined in pn532 User Manual)
 * 
 * @return 0 on success, 
 *         1 on usb error,
 *         2 on bad ccid response format,
 *         3 on memory allocation failure
 */
int pseudo_apdu_GetFirmwareVersion()
{
    unsigned char DirectTransmit[PSEUDO_APDU_DIRECT_TRANSMIT_SIZE];
    int  DirectTransmit_len;
    int ret; 
    READER_RESP *reader; 
    GetFwVerResp *fw_resp;
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }

    fw_resp = (GetFwVerResp *) malloc(sizeof(GetFwVerResp));
    if (!fw_resp)
    {
        printf("Error in alloacating struct for apdu response\n");
        return 3;
    }

    /*Attempt to send Direct transmit command */
    memset(DirectTransmit, 0, sizeof(DirectTransmit));

    /*The following will fill DirectTransmit buffer 
    with the direct transmit apdu containing [d4] [command]
     in the data field*/
    build_apdu_message_get_fw_ver(DirectTransmit, &DirectTransmit_len);

    ccid_set_xfrblocck_frame(DirectTransmit, DirectTransmit_len);

    ret = send_to_reader(ccid_frame_xframe_block, CCID_XFRBLOCK_FIXED_SIZE+DirectTransmit_len);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }

    memset(transfer_buf, 0, sizeof(transfer_buf));
    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }
    
    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        free(fw_resp);
        return 2;
    }

    decode_fw_ver_resp(reader->abData, fw_resp);

    free(reader);
    free(fw_resp);

    return 0;
}


/**
 * @brief This function sends to the reader the Pseudo-APDU Command InListPassiveTarget. In more detail 
 * it embeds insisde a PC_to_RDR_XfrBlock frame the Direct Transmit frame which embeds the pn532 command
 * InListPassiveTarget. 
 * => [PC_to_RDR_XfrBlock[Direct Transmit[InListPassiveTarget]]]
 * (Direct Transmit defined in ACR122U Application Programming Interface section 6.0)
 * (InListPassiveTarget defined in pn532 User Manual)
 * 
 * @return 0 on success (target found), 
 *         1 on usb error,
 *         2 on bad ccid response format,
 *         3 on memory allocation failure
 *         4 if target not found
 */
int pseudo_apdu_InListPassiveTarget()
{
    unsigned char DirectTransmit[PSEUDO_APDU_DIRECT_TRANSMIT_SIZE];
    int  DirectTransmit_len;
    int ret; 
    READER_RESP *reader; 
    InListPsvTrgtResp *list_trgts; 
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }

    list_trgts = (InListPsvTrgtResp *) malloc(sizeof(InListPsvTrgtResp));
    if (!list_trgts)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }


    /*Attempt to send Direct transmit command */
    memset(DirectTransmit, 0, sizeof(DirectTransmit));

    /*The following will fill DirectTransmit buffer 
    with the direct transmit apdu containing [d4] [command]
     in the data field*/
    build_apdu_message_list_passive_targets(DirectTransmit, &DirectTransmit_len);

    ccid_set_xfrblocck_frame(DirectTransmit, DirectTransmit_len);

    ret = send_to_reader(ccid_frame_xframe_block, CCID_XFRBLOCK_FIXED_SIZE+DirectTransmit_len);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }

    memset(transfer_buf, 0, sizeof(transfer_buf));

    //wait enough to get even the time out response ~5ms
    sleep(0.005);

    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }
    
    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        free(list_trgts);
        return 2;
    }

    if (decode_in_list_passive_targets_resp(reader->abData, list_trgts) == 2)
    {
        free(reader);
        free(list_trgts);
        return 4;
    }

    free(reader);
    free(list_trgts);

    return 0;
}

/**
 * @brief This function will poll for contactless tags by sending InListPassiveTarget 
 * every 1 s. It will be blocking until it finds a tag. The logic is that the user is expected 
 * to enter a card.
 * 
 * @return 0 on success, 
 *         1 on usb error,
 *         2 on bad ccid response format,
 *         3 on memory allocation failure
 */
int poll_for_cards()
{
    int ret; 

    while (1)
    {
        ret = pseudo_apdu_InListPassiveTarget();
        if (ret == 4) 
        {
             sleep(1);
              continue;
        }
           
        if (ret != 0)
            return ret;
        if (ret == 0)
            break;  
    }
    return 0;
}

int pseudo_apdu_InDataExchange(char* mif_command, int mif_command_size)
{
    unsigned char DirectTransmit[PSEUDO_APDU_DIRECT_TRANSMIT_SIZE];
    int  DirectTransmit_len;
    int ret; 
    READER_RESP *reader; 
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }


    /*Attempt to send Direct transmit command */
    memset(DirectTransmit, 0, sizeof(DirectTransmit));

    /*The following will fill DirectTransmit buffer 
    with the direct transmit apdu containing [d4] [command]
     in the data field*/
    build_apdu_message_in_data_exchange(DirectTransmit, &DirectTransmit_len, mif_command, 
    mif_command_size);

    ccid_set_xfrblocck_frame(DirectTransmit, DirectTransmit_len);

    ret = send_to_reader(ccid_frame_xframe_block, CCID_XFRBLOCK_FIXED_SIZE+DirectTransmit_len);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }

    memset(transfer_buf, 0, sizeof(transfer_buf));

    //wait enough to get the response
    sleep(0.01);

    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }
    
    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        return 2;
    }

    decode_in_data_exchange(reader->abData);

    free(reader);
    

    return 0;
}

int read_card_memory()
{
    int ret; 
    char read_command[5];

    read_command[0] = 0xd4;
    read_command[1] = 0x40; //in data exchange
    read_command[2] = 0x01; //tag no
    read_command[3] = 0x30;  //read
    read_command[4] = 0x00; // address 0

    printf("Read card dump:\n");
    for (int i=0; i<11; i++)
    {
        pseudo_apdu_InDataExchange(read_command, 5);
        read_command[4] += 4;
    }
    return 0;
}

int authenticate()
{
    int ret;
    char auth_command[20];

    char password[4] = {0x00, 0x00, 0x00, 0x00};
    printf("attemping to auth card with password: %02x %02x %02x %02x\n", 
    password[0], password[1], password[2], password[3]);
    
    /*Here we use InDataExchange
    It won't work because PWD_AUTH is a native command
    and is not understood by pn532 chip*/
    // auth_command[0] = 0xd4;
    // auth_command[1] = 0x40;
    // auth_command[2] = 0x01;
    // auth_command[3] = 0x1b;
    // auth_command[4] = password[0];
    // auth_command[5] = password[1];
    // auth_command[6] = password[2];
    // auth_command[7] = password[3];

    // pseudo_apdu_InDataExchange(auth_command, 8);

    /*Here we use InCommunicateThru*/
    // auth_command[0] = 0xd4;
    // auth_command[1] = 0x42;
    // auth_command[2] = 0x1b;
    // auth_command[3] = password[0];
    // auth_command[4] = password[1];
    // auth_command[5] = password[2];
    // auth_command[6] = password[3];
    // auth_command[7] = 0x8d; 
    // auth_command[8] = 0x00;
//d4  08  63  02  00  63  03  00 
//d4  42  60  f8  32  
    auth_command[0] = 0xd4;
    auth_command[1] = 0x08;
    auth_command[2] = 0x63;
    auth_command[3] = 0x02; 
    auth_command[4] = 0x00;
    auth_command[5] = 0x63;
    auth_command[6] = 0x03;
    auth_command[7] = 0x00;

    pseudo_apdu_InDataExchange(auth_command, 8);

    auth_command[0] = 0xd4;
    auth_command[1] = 0x42;
    auth_command[2] = 0x60;
    auth_command[3] = 0xf8; 
    auth_command[4] = 0x31;
    
    pseudo_apdu_InDataExchange(auth_command, 5);

//1b  41  42  43  44  16  22 
    auth_command[0] = 0xd4;
    auth_command[1] = 0x42;
    auth_command[2] = 0x1b;

    auth_command[3] = 0xff;
    auth_command[4] = 0xff; 
    auth_command[5] = 0xff;
    
     auth_command[6] = 0xff;
    auth_command[7] = 0x00;
    auth_command[8] = 0x00;

    
    pseudo_apdu_InDataExchange(auth_command, 9);
}

int set_configuration()
{
    unsigned char DirectTransmit[PSEUDO_APDU_DIRECT_TRANSMIT_SIZE];
    int  DirectTransmit_len;
    int ret; 
    READER_RESP *reader; 
    unsigned char transfer_buf[256];

    reader = (READER_RESP *) malloc(sizeof(READER_RESP));
    if (!reader)
    {
        printf("Error in alloacating struct for response\n");
        return 3;
    }

    /*Attempt to send Direct transmit command */
    memset(DirectTransmit, 0, sizeof(DirectTransmit));

    /*The following will fill DirectTransmit buffer 
    with the direct transmit apdu containing [d4] [command]
     in the data field*/
    build_apdu_message_configure(DirectTransmit, &DirectTransmit_len);

    ccid_set_xfrblocck_frame(DirectTransmit, DirectTransmit_len);

    ret = send_to_reader(ccid_frame_xframe_block, CCID_XFRBLOCK_FIXED_SIZE+DirectTransmit_len);
    if (ret < 0)
    {
        printf("failed sending command to usb\n");
        return 1;
    }

    memset(transfer_buf, 0, sizeof(transfer_buf));
    ret = receive_from_reader(transfer_buf, sizeof(transfer_buf));
    if (ret < 0)
    {
        printf("failed reading command from usb\n");
        return 1;
    }
    
    if (ccid_parse_rdr_to_pc_datablock(transfer_buf, reader) == 1)
    {
        free(reader);
        return 2;
    }

    free(reader);

    return 0;

}
