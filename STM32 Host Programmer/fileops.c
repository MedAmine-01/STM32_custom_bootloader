#include "main.h"

FILE * file =NULL;


#define USER_APP "./user_app.bin"


uint32_t calc_file_len(void){

    FILE *file;
    uint32_t fileLen;

    file = fopen(USER_APP, "rb");

    if(!file){
        perror("\n\n  bin file not found");
        exit(0);
    }

    fseek(file, 0, SEEK_END);
    fileLen =ftell(file);
    fseek(file, 0, SEEK_SET);

    fclose(file);

    return fileLen;

}

void open_file(void){
    file = fopen(USER_APP, "rb");
    if(!file){
        perror("\n  bin file not found");
        exit(0);
    }
}


uint32_t read_file(uint8_t *pBuffer, uint32_t len){
    return (uint32_t) fread(pBuffer, 1, len, file);
}

void close_file(void){
    fclose(file);
}
