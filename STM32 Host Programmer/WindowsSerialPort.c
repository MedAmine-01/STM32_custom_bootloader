#include "main.h"

HANDLE hComm;

void serial_port_configuration(void){
    char ComPortName[]= "\\\\.\\COM3";
    BOOL status;

    /*--------------Opening the serial port-----------------*/

    hComm = CreateFile(ComPortName,                     //Name of the port to be opened
                       GENERIC_READ | GENERIC_WRITE,    // r/w access
                       0,                               //no sharing (ports can be shared)
                       NULL,                            //no security
                       OPEN_EXISTING,                   //open existing port only
                       0,                               //non overlapped I/O
                       NULL);                           //null for com devices

    if(hComm = INVALID_HANDLE_VALUE){
        printf("\n  Error! - Port %s can't be opened", ComPortName);
        printf("\n  Check board connection and Port number\n");
        exit(-1);
    }
    else{
        printf("\n  Port %s opened\n", ComPortName);
    }

    /*--------------------setting the parameters for the serial port*---------------*/

    DCB dcbSerialParams ={0}; //Initializing DCB structure
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    status = GetCommState(hComm, &dcbSerialParams); // retreives the current setting

    if(status == FALSE){
        printf("\n  Error! in GetCommState()");
    }

    dcbSerialParams.BaudRate = 115200;
    dcbSerialParams.ByteSize =8 ;
    dcbSerialParams.fBinary=1;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    status = SetCommStatus(hComm, &dcbSerialParams); //configuring the port according to the settings above

    if(status == FALSE){
        printf("\n  Error! in DCB structure");
    }
    else{
        printf("\n   Setting DCB structure successfull\n");
        printf("\n       Baudrate = %ld", dcbSerialParams.BaudRate);
        printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
        printf("\n       StopBits = %d", dcbSerialParams.StopBits);
        printf("\n       Parity   = %d", dcbSerialParams.Parity);
    }

    /*----------------------Setting timeouts-----------------------------------*/
    COMMTIMEOUTS timeouts ={0};

    timeouts.ReadIntervalTimeout         = 300;
    timeouts.ReadTotalTimeoutConstant    = 300;
    timeouts.ReadTotalTimeoutMultiplier  = 300;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if(SetCommTimeouts(hComm, &timeouts)==FALSE){
        printf("\n  Error! in setting timeouts");
    }
    else{
        printf("\n\n Setting serial port timeouts successfull");
    }

    status = SetCommMask(hComm, EV_RXCHAR);//Configure windows to Monitor the serial device for Character reception

    if (status == FALSE)
        printf("\n\n   Error! in Setting CommMask");
    else
        printf("\n\n   Setting CommMask successfull");
}

uint32_t read_serial_port(uint8_t *pBuffer, uint32_t len){
    uint32_t read_byte_number;
    ReadFile(hComm, pBuffer, len, &read_byte_number, NULL);
    return read_byte_number;
}

void close_serial_port(void){
    CloseHandle(hComm);
}

void purge_serial_port(void){
    PurgeComm(hComm, PURGE_RXCLEAR|PURGE_TXCLEAR);
}

void write_serial_port(uint8_t *dBuffer, uint32_t len){
    DWORD nbBytesWritten =0;
    BOOL status;

    status = WriteFile(hComm,
                       dBuffer,
                       len,
                       &nbBytesWritten,
                       NULL);
    if(status==TRUE){
        printf("\n  Sending Command!\n");
        for(uint32_t i=0; i<len; i++){
            printf("  0x%2.2x ", dBuffer[i]);
            if(i%8==7){
                printf("\n");
            }
        }
    }
    else{
        printf("\n  Error %ld in writing to serial port", GetLastError());
    }
}

