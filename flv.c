//
// Created by fanxiangyu1 on 2022/6/15.
//
#include <stdio.h>
#include <string.h>
#include "flv.h"


int flv_header_get(unsigned char *buf, _flv_header_t *pflv_header)
{
    if(buf == NULL)
    {
        return -1;
    }
    pflv_header->Signature[0] = buf[0];
    pflv_header->Signature[1] = buf[1];
    pflv_header->Signature[2] = buf[2];
    pflv_header->Version = buf[3];
    pflv_header->TypeFlagsAudio = (buf[4]>>2)&0x01;
    pflv_header->TypeFlagsVideo = (buf[4]>>0)&0x01;
    pflv_header->DataOffset = (buf[5]<<24)|(buf[6]<<16)|(buf[7]<<8)|(buf[8]<<0);
    if(memcmp(pflv_header->Signature,"FLV",sizeof("FLV")-1) != 0)
    {
        return -2;
    }
    return 0;
}
int tag_header_get(unsigned char *buf, _tag_header_t *ptag_header)
{
    if(buf == NULL)
    {
        return -1;
    }
//    int TagType;//1 byte
//    int DataSize;//3 byte
//    int Timestamp;//3 byte
//    int TimeStampExtended;//1 byte
//    int StreamId;//3 byte
    ptag_header->TagType = buf[0];
    ptag_header->DataSize = (buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
    ptag_header->Timestamp = (buf[4]<<16)|(buf[5]<<8)|(buf[6]<<0);
    ptag_header->TimeStampExtended = buf[7];
    ptag_header->StreamId = (buf[8]<<16)|(buf[9]<<8)|(buf[10]<<0);

    if(ptag_header->TagType != TagAudio
        && ptag_header->TagType != TagVideo
           && ptag_header->TagType != TagScriptData)
    {
        return -2;
    }
    return 0;
}

int audio_type_print(uint8 SoundFormat)
{
    uint8 audiotag_str[64]={0};
    switch(SoundFormat)
    {
        case 0:strcat(audiotag_str,"Linear PCM, platform endian");break;
        case 1:strcat(audiotag_str,"ADPCM");break;
        case 2:strcat(audiotag_str,"MP3");break;
        case 3:strcat(audiotag_str,"Linear PCM, little endian");break;
        case 4:strcat(audiotag_str,"Nellymoser 16-kHz mono");break;
        case 5:strcat(audiotag_str,"Nellymoser 8-kHz mono");break;
        case 6:strcat(audiotag_str,"Nellymoser");break;
        case 7:strcat(audiotag_str,"G.711 A-law logarithmic PCM");break;
        case 8:strcat(audiotag_str,"G.711 mu-law logarithmic PCM");break;
        case 9:strcat(audiotag_str,"reserved");break;
        case 10:strcat(audiotag_str,"AAC");break;
        case 11:strcat(audiotag_str,"Speex");break;
        case 14:strcat(audiotag_str,"MP3 8-Khz");break;
        case 15:strcat(audiotag_str,"Device-specific sound");break;
        default:strcat(audiotag_str,"unknown");break;
    }
    printf("audio type=%s\n",audiotag_str);
    return 0;
}

int audio_header_get(unsigned char *buf, _audio_tag_header *paudio_header)
{
    if(buf == NULL)
    {
        return -1;
    }
    paudio_header->SoundFormat = (buf[0]>>4)&0x0f;
    paudio_header->SoundRate = (buf[0]>>2)&0x03;
    paudio_header->SoundSize = (buf[0]>>1)&0x01;
    paudio_header->SoundType = (buf[0]>>0)&0x01;

    audio_type_print(paudio_header->SoundFormat);
    return 0;
}



