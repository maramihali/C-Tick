#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IP_HLEN(lenver) (lenver & 0x0f)
#define TCP_HLEN(offres) (offres >> 4)

long extract_data(unsigned const char *buffer, long buffer_len, unsigned char *data){

    uint8_t first_ihl =  (uint8_t)IP_HLEN(buffer[0]);
    uint16_t first_ip_len = (buffer[2] << 8) + buffer[3];
    uint8_t first_tcp_offset = TCP_HLEN(buffer[first_ihl * 4 + 12]);
    uint16_t data_len = first_ip_len - (uint16_t )(first_ihl * 4) - (uint16_t )(first_tcp_offset * 4);
    long offset = first_ip_len;
    int index = 0;
    long total_data_len = data_len;

    if(data_len > 0){
        uint16_t start = (uint16_t )(first_ihl * 4) + (uint16_t )(first_tcp_offset * 4);
        while(start < data_len){
            data[index ++] = buffer[start ++];
        }
    }

    while(offset < buffer_len){

        uint8_t curr_ihl = (uint8_t) IP_HLEN(buffer[offset]);
        uint16_t curr_len = (buffer[offset + 2] << 8) + buffer[offset + 3];
        uint8_t curr_tcp_offset = TCP_HLEN(buffer[offset + curr_ihl * 4 + 12]);
        data_len = curr_len - (uint16_t )(curr_ihl * 4) - (uint16_t )(curr_tcp_offset * 4);

        if(data_len > 0){
            uint16_t start = (uint16_t )offset + (uint16_t )(curr_ihl * 4) + (uint16_t )(curr_tcp_offset * 4);
            while(start < offset + curr_len){
                data[index ++] = buffer[start ++];
            }
        }

        total_data_len += data_len;

        offset += curr_len;
    }


    return total_data_len;
}


int main(int argc, char *argv[]){

    FILE *file_in_ptr;
    FILE *file_out_ptr;
    unsigned char *buffer;
    unsigned char *extracted_data;
    long file_len;

    if(argc != 3){
        puts("Usage : extract <srcfile> <destfile>");
        return 1;
    }

    if((file_in_ptr=fopen(argv[1],"rb")) == NULL){
        perror("Cannot find file to serve");
        return 2;
    }

    if((file_out_ptr = fopen(argv[2],"w")) == NULL){
        perror("Error occurred while opening the destination file");
        return 3;
    }

    fseek(file_in_ptr,0, SEEK_END);
    file_len = ftell(file_in_ptr);
    rewind(file_in_ptr);

    buffer = (unsigned char *)malloc((file_len + 1)*sizeof(char));
    extracted_data = (unsigned char *)malloc((file_len + 1)*sizeof(char));
    fread(buffer,file_len,1,file_in_ptr);
    fclose(file_in_ptr);

    long data_len = extract_data(buffer, file_len,extracted_data);

    printf("%s",extracted_data);

    fwrite(extracted_data, sizeof(char),data_len,file_out_ptr);
    fclose(file_out_ptr);

    free(buffer);
    free(extracted_data);

    return 0;
}
