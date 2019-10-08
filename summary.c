#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define IP_HLEN(lenver) (lenver & 0x0f)
#define TCP_HLEN(offres) (offres >> 4)

char *get_IP_address(unsigned char *buffer, int start){
    char *result = (char*)malloc(16 * sizeof(char));
    sprintf(result, "%d.%d.%d.%d", buffer[start], buffer[start+1], buffer[start+2], buffer[start+3]);
    return result;
}

int get_packets_number(unsigned const char *buffer, uint16_t first_pack_len, long buffer_len){
    int packets_no = 1;
    long offset = first_pack_len;
    while(offset < buffer_len) {
        uint16_t curr_pack_len = (buffer[offset + 2] << 8) + buffer[offset + 3];
        printf("%d ", curr_pack_len);
        packets_no ++;
        offset += curr_pack_len;
    }
    return packets_no;
}

int main(int argc, char *argv[]){

    FILE *fileptr;
    unsigned char *buffer;
    long file_len;
    char *src, *dest;

    if (argc != 2) {
        puts("Usage: summary <file>");
        return 1;
    }

    if ((fileptr=fopen(argv[1],"rb")) == NULL) {
        perror("Cannot find file to serve");
        return 2;
    }

    fseek(fileptr,0, SEEK_END);
    file_len = ftell(fileptr);
    rewind(fileptr);

    buffer = (unsigned char *)malloc((file_len + 1)*sizeof(char));
    fread(buffer,file_len,1,fileptr);
    fclose(fileptr);
  
    src = get_IP_address(buffer,12);
    dest = get_IP_address(buffer,16);
    uint8_t first_ihl =  (uint8_t)IP_HLEN(buffer[0]);
    uint16_t first_ip_len = (buffer[2] << 8) + buffer[3];
    uint8_t first_tcp_offset = TCP_HLEN(buffer[first_ihl * 4 + 12]);
    int packets_no = get_packets_number(buffer, first_ip_len, file_len);
    
    printf("%s %s %d %d %d %d\n", src, dest, first_ihl, first_ip_len, first_tcp_offset, packets_no);

    free(buffer);
    free(src);
    free(dest);

    return 0;

}

