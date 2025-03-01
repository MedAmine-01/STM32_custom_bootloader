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



uint8_t supported_commands[]={
		BL_GET_VER,
		BL_GET_HELP,
		BL_GET_CID,
		BL_GET_RDP_STATUS,
		BL_GO_TO_ADDRESS,
		BL_FLASH_ERASE,
		BL_MEM_WRITE,
		BL_READ_SECTOR_STATUS
};
void bootloader_handle_gethelp_cmd(char * bl_rx_buffer){
		uint32_t command_packet_len = bl_rx_buffer[0]+1;

		uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

		if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
			bootloader_send_ack(bl_rx_buffer[0],sizeof(supported_commands));
			bootloader_uart_write_data(supported_commands,sizeof(supported_commands));
		}
		else{
			bootloader_send_nack();
		}
}

static uint16_t get_chip_id(){
	uint16_t cid;
	cid = (uint16_t) (DBGMCU->IDCODE) & 0x0FFF;
	return cid;
}

void bootloader_handle_getcid_cmd(char * bl_rx_buffer){
			uint32_t command_packet_len = bl_rx_buffer[0]+1;

			uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

			if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
				uint16_t cid = get_chip_id();
				bootloader_send_ack(bl_rx_buffer[0],2);
				bootloader_uart_write_data((uint8_t *)&cid,2);
			}
			else{
				bootloader_send_nack();
			}
}

static uint8_t get_flash_rdp_level(){
	uint8_t rdp_status=0;
	volatile uint32_t *pOB_addr = (uint32_t *) 0x1FFFC000;
	rdp_status = (uint8_t) (*pOB_addr>>8);
	return rdp_status;
}

void bootloader_handle_getrdpstatus(uint8_t * bl_rx_buffer){
	uint32_t command_packet_len = 5;

	uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

	if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
		uint8_t rdp_level = get_flash_rdp_level();
		bootloader_send_ack(bl_rx_buffer[0],1);
		bootloader_uart_write_data((uint8_t *)&rdp_level,1);
	}
	else{
		bootloader_send_nack();
	}
}

//TODO implement later address verification (now it's trust and jump)
static uint8_t verify_address(){
	return ADDRESS_VALID;
}

void bootloader_handle_gotoaddress_cmd(char * bl_rx_buffer){
	uint32_t go_to_address =0;
	uint32_t command_packet_len = bl_rx_buffer[0]+1;

	uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

	if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
		bootloader_send_ack(bl_rx_buffer[0],1);
		go_to_address= *((uint32_t *) &bl_rx_buffer[2]);
		if(verify_address(go_to_address)==ADDRESS_VALID){
			uint8_t addr_valid = ADDRESS_VALID;
			bootloader_uart_write_data(&addr_valid, 1);
			//TRUST AND JUMP LOL :D
			//go_to_address+=1;//make T Bit
			void (*jump_addr)(void)= (void*)go_to_address;
			//call the jump function
			jump_addr();
		}
		else{
			uint8_t addr_invalid = ADDRESS_INVALID;
			bootloader_uart_write_data(&addr_invalid, 1);
		}
	}
	else{
		bootloader_send_nack();
	}
}

static uint8_t execute_flash_erase(uint8_t first_sector, uint8_t number_of_sectors){
	FLASH_EraseInitTypeDef flash_erase_handle;
	uint32_t SectorError;
	uint8_t status;
	if(number_of_sectors>8){
		return 0;
	}
	if(first_sector==0xFF){
		flash_erase_handle.TypeErase = FLASH_TYPEERASE_MASSERASE;
	}
	else{
		 uint8_t remaining_sectors = 8 - number_of_sectors;
		 if(number_of_sectors>remaining_sectors){
			 number_of_sectors=remaining_sectors;
		 }
		 flash_erase_handle.TypeErase= FLASH_TYPEERASE_SECTORS;
		 flash_erase_handle.Sector=first_sector;
		 flash_erase_handle.NbSectors=number_of_sectors;
	}
	flash_erase_handle.Banks=FLASH_BANK_1;
	HAL_FLASH_Unlock();
	flash_erase_handle.VoltageRange=FLASH_VOLTAGE_RANGE_3;
	status=(uint8_t) HAL_FLASHEx_Erase(&flash_erase_handle, &SectorError);
	HAL_FLASH_Lock();
	return status;
}

void bootloader_handle_flasherase_cmd(char * bl_rx_buffer){
	uint8_t erase_status=0;
	uint32_t command_packet_len = bl_rx_buffer[0]+1;

	uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

	if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
		bootloader_send_ack(bl_rx_buffer[0],1);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		erase_status=execute_flash_erase(bl_rx_buffer[2], bl_rx_buffer[3]);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		bootloader_uart_write_data(&erase_status,1);
	}
	else{
		bootloader_send_nack();
	}
}



//this function implements only writing to flash memory
static uint8_t execute_memory_write(uint8_t * source , uint8_t *destination, uint8_t len){
	uint8_t status = HAL_OK;
	HAL_FLASH_Unlock();
	for(uint32_t i=0;i<len;i++){
		status=HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, destination+i, source[i]);
	}
	HAL_FLASH_Lock();
	return status;
}

void bootloader_handle_memwrite_cmd(char * bl_rx_buffer){
	uint32_t command_packet_len = bl_rx_buffer[0]+1;

	uint32_t memory_address=0;
	uint8_t len = bl_rx_buffer[0], checksum;
	uint8_t playload_len = bl_rx_buffer[6];
	uint8_t write_status;
	checksum=bl_rx_buffer[len];

	uint32_t host_crc = * ((uint32_t *) (bl_rx_buffer+command_packet_len-4));

	//if(bootloader_verify_crc(&bl_rx_buffer[0],command_packet_len-4,host_crc)){
		bootloader_send_ack(bl_rx_buffer[0],1);
		memory_address= *((uint32_t *) &bl_rx_buffer[2]);
		if(verify_address(memory_address)==ADDRESS_VALID){
					write_status= execute_memory_write(&bl_rx_buffer[7],memory_address,playload_len);
					bootloader_uart_write_data(&write_status, 1);
		}
		else{

		}
	/*}
	else{
		bootloader_send_ack(&write_status, 1);
	}*/
}
void bootloader_handle_disrwprotect_cmd(char * bl_rx_buffer){}
void bootloader_handle_otpread_cmd(char * bl_rx_buffer){}
void bootloader_handle_readsectorstatus_cmd(char * bl_rx_buffer){}
void bootloader_handle_memread_cmd(char * bl_rx_buffer){}
void bootloader_handle_enrwprotect_cmd(char * bl_rx_buffer){}
