#include "bootloader_commands.h"


static uint8_t get_bootloader_version(){
	return BL_VERSION;
}

static void bootloader_uart_write_data(uint8_t *pBuffer, uint32_t len){
	HAL_UART_Transmit(&huart2, pBuffer, len, HAL_MAX_DELAY);
}

static void bootloader_send_nack(void){
	uint8_t nack = NACK;
	HAL_UART_Transmit(&huart2, &nack, 1, HAL_MAX_DELAY);
}

static void bootloader_send_ack(uint8_t command_code, uint8_t follow_length){
	uint8_t ack_buff[2];
	ack_buff[0]=ACK;
	ack_buff[1]=follow_length;
	HAL_UART_Transmit(&huart2, ack_buff, 2, HAL_MAX_DELAY);
}

static uint8_t bootloader_verify_crc(uint8_t * pData, uint32_t len, uint32_t crc_host){
	uint32_t uwCRCvalue=0xff;

	for(uint32_t i=0;i<len;i++){
		uint32_t i_data = pData[i];
		uwCRCvalue = HAL_CRC_Accumulate(&hcrc, &i_data, 1);
	}
	if(uwCRCvalue==crc_host){
		return VERIFY_CRC_SUCCESS;
	}
	else{
		return VERIFY_CRC_FAIL;
	}
}

void bootloader_handle_getver_cmd(char * bl_rx_buffer){
	uint8_t bl_version;

	uint32_t command_packet_len = bl_rx_buffer[0]+1;

	uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

	if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
		bootloader_send_ack(bl_rx_buffer[0],1);
		bl_version = get_bootloader_version();
		bootloader_uart_write_data(&bl_version,1);
	}
	else{
		bootloader_send_nack();
	}
}
void bootloader_handle_memwrite_cmd(char * bl_rx_buffer){}
void bootloader_handle_flasherase_cmd(char * bl_rx_buffer){}
void bootloader_handle_gotoaddress_cmd(char * bl_rx_buffer){}
void bootloader_handle_getrdpstatus(){}
void bootloader_handle_getcid_cmd(char * bl_rx_buffer){}
void bootloader_handle_gethelp_cmd(char * bl_rx_buffer){}
void bootloader_handle_disrwprotect_cmd(char * bl_rx_buffer){}
void bootloader_handle_otpread_cmd(char * bl_rx_buffer){}
void bootloader_handle_readsectorstatus_cmd(char * bl_rx_buffer){}
void bootloader_handle_memread_cmd(char * bl_rx_buffer){}
void bootloader_handle_enrwprotect_cmd(char * bl_rx_buffer){}
