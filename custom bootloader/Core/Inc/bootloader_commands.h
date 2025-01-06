/*
 * bootloader_command.h
 *
 *  Created on: Jan 6, 2025
 *      Author: user
 */

#ifndef INC_BOOTLOADER_COMMANDS_H_
#define INC_BOOTLOADER_COMMANDS_H_

#define BL_OTP_READ 				0x27
#define BL_READ_SECTOR_STATUS		0x28
#define BL_MEM_READ 				0x29
#define BL_EN_R_W_PROTECT			0x2A
#define BL_MEM_WRITE				0x2B
#define BL_FLASH_ERASE				0x2C
#define BL_GO_TO_ADDRESS			0x2D
#define BL_GET_RDP_STATUS			0x2E
#define BL_GET_CID					0x2F
#define BL_GET_HELP					0x30
#define BL_GET_VER					0x31
#define BL_DIS_R_W_PROTECT			0x32


void bootloader_handle_memwrite_cmd();
void bootloader_handle_flasherase_cmd();
void bootloader_handle_gotoaddress_cmd();
void bootloader_handle_getrdpstatus();
void bootloader_handle_getcid_cmd();
void bootloader_handle_gethelp_cmd();
void bootloader_handle_getver_cmd();
void bootloader_handle_disrwprotect_cmd();
void bootloader_handle_otpread_cmd();
void bootloader_handle_readsectorstatus_cmd();
void bootloader_handle_memread_cmd();
void bootloader_handle_enrwprotect_cmd();


#endif /* INC_BOOTLOADER_COMMANDS_H_ */
