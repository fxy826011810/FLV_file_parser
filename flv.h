//
// Created by fanxiangyu1 on 2022/6/15.
//

#ifndef FLV_FLV_H
#define FLV_FLV_H

#define FLV_HEADER_SIZE (9)
#define TAG_HEADER_SIZE (11)
#define AUDIO_HEADER_SIZE (8)

typedef unsigned int    uint32;
typedef unsigned short  uint16;
typedef unsigned char   uint8;

typedef int    int32;
typedef short  int16;
typedef char   int8;

typedef struct{
    uint8 Signature[3];
    uint8 Version;
//    char TypeFlagsReserved; //5bit
    uint8 TypeFlagsAudio;//1bit
//    char TypeFlagsReserved; //5bit
    uint8 TypeFlagsVideo;//1bit
    uint32 DataOffset;
}_flv_header_t;

typedef enum{
    TagAudio=0x08,
    TagVideo=0x09,
    TagScriptData=0x12,
}_tag_type_e;

typedef struct{
    _tag_type_e TagType;//1 byte
    uint32 DataSize;//3 byte
    uint32 Timestamp;//3 byte
    uint32 TimeStampExtended;//1 byte
    uint32 StreamId;//3 byte
}_tag_header_t;


int flv_header_get(unsigned char *buf, _flv_header_t *pflv_header);
int tag_header_get(unsigned char *buf, _tag_header_t *ptag_header);

typedef struct{
    uint8 SoundFormat;//4 bit
    uint8 SoundRate;//2 bit
    uint8 SoundSize;//1 bit
    uint8 SoundType;//1 bit
}_audio_tag_header;

int audio_header_get(unsigned char *buf, _audio_tag_header *paudio_header);

typedef enum{
    AMF_DATA_TYPE_NUMBER = 0x00,
    AMF_DATA_TYPE_BOOL,
    AMF_DATA_TYPE_STRING,
    AMF_DATA_TYPE_OBJECT,
    AMF_DATA_TYPE_NULL,
    AMF_DATA_TYPE_UNDEFINE,
    AMF_DATA_TYPE_REFERENCE,
    AMF_DATA_TYPE_MIXEDARRAY,
    AMF_DATA_TYPE_OBJECT_END,
    AMF_DATA_TYPE_ARRAY,
    AMF_DATA_TYPE_DATE,
    AMF_DATA_TYPE_LONG_STRING,
    AMF_DATA_TYPE_UNSUPPORTED,
}_amf_type_e;

#endif //FLV_FLV_H
