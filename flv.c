//
// Created by fanxiangyu1 on 2022/6/15.
//
#include <stdio.h>
#include <string.h>
#include "flv.h"


int flv_header_get(unsigned char *buf, _flv_header_t *pflv_header)
{
    if(buf == NULL || pflv_header == NULL)
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
    if(buf == NULL || ptag_header == NULL)
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

int aac_pkg_type_parse(unsigned char *buf,_aac_audio_data_t *paac_audio_data)
{
    {
        paac_audio_data->AACPacketType = buf[0];
//        0 = AAC sequence header
//        1 = AAC raw
        if(paac_audio_data->AACPacketType == 0)
        {
            printf("aac audio type=AAC sequence header\n");
        }
        else if(paac_audio_data->AACPacketType == 1)
        {
            printf("aac audio type=AAC raw\n");
        }
    }
    return 0;
}

int audio_header_get(unsigned char *buf, _audio_tag_header *paudio_header)
{
    if(buf == NULL || paudio_header == NULL)
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

int aac_AudioSpecificConfig_parse(unsigned char *buf,_aac_AudioSpecificConfig_t *pAudioSpecificConfig)
{
    pAudioSpecificConfig->audioObjectType = (buf[0]>>3)&0x1f;
    pAudioSpecificConfig->samplingFrequencyIndex = ((buf[0] & 0x7) << 1) | (buf[1] >> 7);
    pAudioSpecificConfig->channelConfiguration =  (buf[1] >> 3) & 0x0F;
    pAudioSpecificConfig->frameLengthFlag = (buf[1] >> 2) & 0x01;
    pAudioSpecificConfig->dependsOnCoreCoder =  (buf[1] >> 1) & 0x01;
    pAudioSpecificConfig->extensionFlag = buf[1] & 0x01;
    return 0;
}

int video_frame_type_print(_VIDEO_FRAME_TYPE_E FrameType)
{
    uint8 videotag_str[64]={0};
    switch(FrameType)
    {
        case VIDEO_FRAME_TYPE_KEY_FRAME:strcat(videotag_str,"key frame (for AVC, a seekable frame)");break;
        case VIDEO_FRAME_TYPE_INTER_FRAME:strcat(videotag_str,"inter frame (for AVC, a non-seekable frame)");break;
        case VIDEO_FRAME_TYPE_DISPOSABLE_INTER_FRAME:strcat(videotag_str,"disposable inter frame (H.263 only)");break;
        case VIDEO_FRAME_TYPE_GENERATED_KEY_FRAME:strcat(videotag_str,"generated key frame (reserved for server use only)");break;
        case VIDEO_FRAME_TYPE_VIDEO_INFO_COMMAND_FRAME:strcat(videotag_str,"video info/command frame");break;
        default:strcat(videotag_str,"unknown");break;
    }
    printf("video frame type=%s\n",videotag_str);
    return 0;
}
int video_codec_id_print(_VIDEO_FRAME_TYPE_E FrameType)
{
    uint8 videotag_str[64]={0};
    switch(FrameType)
    {
        case VIDEO_CODEC_JPEG:strcat(videotag_str,"JPEG");break;
        case VIDEO_CODEC_H263:strcat(videotag_str,"Sorenson H.263");break;
        case VIDEO_CODEC_SCREEN_VIDEO:strcat(videotag_str,"Screen video");break;
        case VIDEO_CODEC_ON2_VP6:strcat(videotag_str,"On2 VP6");break;
        case VIDEO_CODEC_ON2_VP6_WITH_ALPHA:strcat(videotag_str,"On2 VP6 with alpha channel");break;
        case VIDEO_CODEC_SCREEN_VIDEO_V2:strcat(videotag_str,"Screen video version 2");break;
        case VIDEO_CODEC_AVC:strcat(videotag_str,"AVC");break;
        default:strcat(videotag_str,"unknown");break;
    }
    printf("video codec type=%s\n",videotag_str);
    return 0;
}
int video_header_get(unsigned char *buf, _video_tag_header *pvideo_header)
{
    if(buf == NULL || pvideo_header == NULL)
    {
        return -1;
    }
    pvideo_header->FrameType = (_VIDEO_FRAME_TYPE_E)((buf[0]>>4)&0x0f);
    pvideo_header->CodecID = (_VIDEO_CODEC_ID_E)((buf[0])&0x0f);
    video_frame_type_print(pvideo_header->FrameType);
    video_codec_id_print(pvideo_header->CodecID);
    return 0;
}


int video_avc_packet_type_print(_AVCPacketType_E AVCPacketType)
{
    uint8 videotag_str[128]={0};
    switch(AVCPacketType)
    {
        case AVC_SEQUENCE_HEADER:strcat(videotag_str,"AVC sequence header");break;
        case AVC_NALU:strcat(videotag_str,"AVC NALU");break;
        case AVC_SEQUENCE_END:strcat(videotag_str,"AVC end of sequence (lower level NALU sequence ender is\n"
                                                  "not required or supported)");break;
        default:strcat(videotag_str,"unknown");break;
    }
    printf("avc packet type =%s\n",videotag_str);
    return 0;
}
int video_avc_header_get(unsigned char *buf, _avc_video_packet_t *pavc_video_header)
{
    if(buf == NULL || pavc_video_header == NULL)
    {
        return -1;
    }
    pavc_video_header->AVCPacketType = buf[0];
    pavc_video_header->CompositionTime = (buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
    video_avc_packet_type_print(pavc_video_header->AVCPacketType);
    return 0;
}

void AMP_STRING_LEN(uint8 *pbuf,uint32 *plen){
    (*plen) += (pbuf[0]<<8)|(pbuf[1]<<0);
    (*plen) += sizeof(uint16);

    uint8 str[512]={0};
    uint32 StringLength = (pbuf[0]<<8)|(pbuf[1]<<0);
    memcpy(str,&pbuf[2],StringLength);
//    printf("string=%s\n",str);
}
void AMP_LONG_STRING_LEN(uint8 *pbuf,uint32 *plen){
    (*plen) += (pbuf[0]<<24)|(pbuf[1]<<16)|(pbuf[2]<<8)|(pbuf[3]<<0);
    (*plen) += sizeof(uint32);
}

void AMP_OBJECT_LEN(uint8 *pbuf,uint32 *plen){
    uint32 offset = 0;
    AMP_STRING_LEN(&pbuf[offset],plen);
    (*plen) += sizeof(uint8);
}
void AMP_OBJECT_END_LEN(uint8 *pbuf,uint32 *plen){
//    uint32 offset = 0;
//    AMP_STRING_LEN(&pbuf[offset],plen);
    (*plen) += 3;
}

uint32 AMF_data_len_get(unsigned char *buf,_amf_type_e data_type);

typedef struct{
    _amf_type_e data_type;
    uint32 data_len;
}_amf_data_unit_t;

uint32 AMF_data_printf(unsigned char *buf,_amf_data_unit_t *punit)
{
    uint8 str[1024]={0};
    switch(punit->data_type)
    {
        case AMF_DATA_TYPE_NUMBER:{
            str[0] = buf[7];
            str[1] = buf[6];
            str[2] = buf[5];
            str[3] = buf[4];
            str[4] = buf[3];
            str[5] = buf[2];
            str[6] = buf[1];
            str[7] = buf[0];
            printf("num=%f\n",*(DOUBLE *)str);
            break;
        }
        case AMF_DATA_TYPE_BOOL:{
            memcpy(str,buf,punit->data_len);
            printf("bool=%d\n",*(uint8 *)str);
            break;
        }
        case AMF_DATA_TYPE_STRING:{
            uint32 StringLength = (buf[0]<<8)|(buf[1]<<0);
            memcpy(str,&buf[2],StringLength);
            printf("datal_string=%s\n",str);
            break;
        }
        case AMF_DATA_TYPE_OBJECT:{
//            data_len = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
//            data_len += sizeof(uint32);
            break;
        }
        case AMF_DATA_TYPE_MovieClip:{
            ;
            break;
        }
        case AMF_DATA_TYPE_NULL:{
            ;
            break;
        }
        case AMF_DATA_TYPE_UNDEFINE:{
            ;
            break;
        }
        case AMF_DATA_TYPE_REFERENCE:{
//            data_len += sizeof(uint16);
            break;
        }
        case AMF_DATA_TYPE_ECMA_array:{
//            data_len = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
//            data_len += sizeof(uint32);
            break;
        }
        case AMF_DATA_TYPE_OBJECT_END:{
            ;
            break;
        }
        case AMF_DATA_TYPE_ARRAY:{
//            data_len = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
//            data_len += sizeof(uint32);
            break;
        }
        case AMF_DATA_TYPE_DATE:{
//            data_len += sizeof(DOUBLE);
//            data_len += sizeof(int16);
            break;
        }
        case AMF_DATA_TYPE_LONG_STRING:{
//            data_len = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
//            data_len += sizeof(uint32);
            break;
        }
        default:
            ;break;
    }
    return 0;
}

uint32 AMF_data_len_get(unsigned char *buf,_amf_type_e data_type)
{
    uint32 data_len = 0;
    switch(data_type)
    {
        case AMF_DATA_TYPE_NUMBER:{
            data_len += sizeof(DOUBLE);
            _amf_data_unit_t unit={
                    .data_type = data_type,
                    .data_len = data_len,
            };
            AMF_data_printf(&buf[0],&unit);
            break;
        }
        case AMF_DATA_TYPE_BOOL:{
            data_len += sizeof(uint8);
            _amf_data_unit_t unit={
                    .data_type = data_type,
                    .data_len = data_len,
            };
            AMF_data_printf(&buf[0],&unit);
            break;
        }
        case AMF_DATA_TYPE_STRING:{
            AMP_STRING_LEN(buf,&data_len);
            _amf_data_unit_t unit={
                    .data_type = data_type,
                    .data_len = data_len,
            };
            AMF_data_printf(&buf[0],&unit);
            break;
        }
        case AMF_DATA_TYPE_OBJECT:{
            AMP_OBJECT_LEN(buf,&data_len);
            break;
        }
        case AMF_DATA_TYPE_MovieClip:{
            ;
            break;
        }
        case AMF_DATA_TYPE_NULL:{
            ;
            break;
        }
        case AMF_DATA_TYPE_UNDEFINE:{
            ;
            break;
        }
        case AMF_DATA_TYPE_REFERENCE:{
            data_len += sizeof(uint16);
            break;
        }
        case AMF_DATA_TYPE_ECMA_array:{
            uint32 ECMAArrayLength = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
            data_len += 4;
            while(ECMAArrayLength)
            {
                uint32 len = 0;
                uint8 *ptmp = &buf[data_len];
                len = AMF_data_len_get(ptmp,AMF_DATA_TYPE_STRING);
                data_len += len;
                ptmp = &buf[data_len++];
                len = AMF_data_len_get(&ptmp[1],ptmp[0]);
                data_len += len;
                ECMAArrayLength--;
            }
            AMP_OBJECT_END_LEN(&buf[data_len],&data_len);
            break;
        }
        case AMF_DATA_TYPE_OBJECT_END:{
            ;
            break;
        }
        case AMF_DATA_TYPE_ARRAY:{
            data_len = (buf[0]<<24)|(buf[1]<<16)|(buf[2]<<8)|(buf[3]<<0);
            data_len += sizeof(uint32);
            break;
        }
        case AMF_DATA_TYPE_DATE:{
            data_len += sizeof(DOUBLE);
            data_len += sizeof(int16);
            break;
        }
        case AMF_DATA_TYPE_LONG_STRING:{
            AMP_LONG_STRING_LEN(buf,&data_len);
            break;
        }
        default:data_len = 0;break;
    }
    return data_len;
}

int AMF_data_parse(unsigned char *buf,uint32 max_len)
{
    uint32 offset = 0;
    _amf_data_unit_t unit = {0};
    while(offset<max_len)
    {
        unit.data_type = buf[offset++];
        unit.data_len = AMF_data_len_get(&buf[offset],unit.data_type);
        AMF_data_printf(&buf[offset],&unit);
        offset+=unit.data_len;
    }
    return 0;
}

int adts_header_construct_to_buf(_adts_header_construct_t *pconstrcut,_audio_tag_header *paudio_header,uint8 *pbuf)
{
    pbuf[0] = (pconstrcut->syncword >> 4)&0xff;
    pbuf[1] = ((pconstrcut->syncword << 4)&0xf0)
                |((pconstrcut->id&0x01)<<3)
                |((pconstrcut->layer&0x03)<<1)
                |(pconstrcut->protection_absent&0x01);
    pbuf[2] = ((pconstrcut->profile&0x03)<<6)
                |((pconstrcut->sampling_frequency_index&0x0f)<<2)
                |((pconstrcut->private_bit&0x01)<<1)
                |((pconstrcut->channel_configuration>>2)&0x01);
    pbuf[3] = ((pconstrcut->channel_configuration&0x03)<<6)
                |((pconstrcut->original_copy&0x01)<<5)
                |((pconstrcut->home&0x01)<<4)
                |((pconstrcut->copyright_identification_bit&0x01)<<3)
                |((pconstrcut->copyright_identification_start&0x01)<<2)
                |((pconstrcut->aac_frame_length>>11)&0x03);
    pbuf[4] = ((pconstrcut->aac_frame_length>>3)&0xff);
    pbuf[5] = ((pconstrcut->aac_frame_length&0x07)<<5)
                |((pconstrcut->adts_buffer_fullness>>6)&0x1F);
    pbuf[6] = ((pconstrcut->adts_buffer_fullness&0x3f)<<2)
                |(pconstrcut->number_of_raw_data_blocks_in_frame&0x03);
    return 0;
}


