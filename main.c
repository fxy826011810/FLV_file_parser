#include <stdio.h>
#include "flv.h"

#define FLV_FILE "D:\\\\flv\\461480359_nb2-1-120.flv"

int main() {

    FILE *pflv = NULL;
    uint8 flv_header_buf[FLV_HEADER_SIZE]={0};
    uint8 tag_header_buf[TAG_HEADER_SIZE]={0};

    printf("uint8=%d uint16=%d uint32=%d\n",sizeof(uint8),sizeof(uint16),sizeof(uint32));

    pflv = fopen(FLV_FILE,"rb+");
    if(pflv == NULL)
    {
        printf("file open error\n");
        return -1;
    }
    fread(flv_header_buf,1,FLV_HEADER_SIZE,pflv);

    _flv_header_t flv_header = {0};
    if(0 != flv_header_get(flv_header_buf,&flv_header))
    {
        printf("flv header get error\n");
        return -2;
    }
    fseek(pflv, flv_header.DataOffset, SEEK_SET);

    uint8 PreviousTagSize[4] = {0};

    _tag_header_t tag_header = {0};
    unsigned int read_len = 0;
    do{
        read_len = fread(PreviousTagSize,1,sizeof(PreviousTagSize),pflv);
        if(read_len!=sizeof(PreviousTagSize))
        {
            printf("read PreviousTagSize error\n");
            return -3;
        }
        read_len = fread(tag_header_buf,1,sizeof(tag_header_buf),pflv);
        if(read_len!=sizeof(tag_header_buf))
        {
            printf("read tag header error\n");
            return -4;
        }
        if(0 != tag_header_get(tag_header_buf,&tag_header))
        {
            printf("tag header get error\n");
            return -5;
        }
        uint32 data_size = tag_header.DataSize;
        if(tag_header.TagType == TagAudio)
        {
            unsigned char audio_header_buf[1]={0};
            _audio_tag_header audio_tag_header={0};
            read_len = fread(audio_header_buf,1,1,pflv);
            data_size -= read_len;
            if(audio_header_get(audio_header_buf,&audio_tag_header) == 0)
            {

            }
        }
        else if(tag_header.TagType == TagVideo)
        {

        }
        else if(tag_header.TagType == TagScriptData)
        {

        }
        fseek(pflv, data_size, SEEK_CUR);
    }while (!feof(pflv));

    return 0;
}
