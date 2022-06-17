#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        unsigned char *ptag_data = malloc((data_size+4)/4*4);
        if(ptag_data == NULL)
        {
            fseek(pflv, data_size, SEEK_CUR);
            goto next;
        }

        read_len = fread(ptag_data,1,data_size,pflv);
        if(read_len != data_size)
        {
            goto next;
        }
        if(tag_header.TagType == TagAudio)
        {
            _audio_tag_header audio_tag_header={0};
            if(audio_header_get(ptag_data,&audio_tag_header) == 0)
            {
                audio_type_parse(&ptag_data[1],audio_tag_header.SoundFormat);
            }
        }
        else if(tag_header.TagType == TagVideo)
        {
            _video_tag_header video_header={0};
            video_header_get(ptag_data,&video_header);
            if(video_header.CodecID == VIDEO_CODEC_AVC)
            {
                _avc_video_packet_t avc_video_header={0};
                video_avc_header_get(&ptag_data[1],&avc_video_header);
            }
        }
        else if(tag_header.TagType == TagScriptData)
        {
            AMF_data_parse(ptag_data,data_size);
        }
        next:
        if(ptag_data)
        {
            free(ptag_data);
            ptag_data = NULL;
        }
    }while (!feof(pflv));

    return 0;
}
