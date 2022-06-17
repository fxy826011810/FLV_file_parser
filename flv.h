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

typedef double DOUBLE;

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
int audio_type_parse(unsigned char *buf,uint8 SoundFormat);

typedef enum{
    VIDEO_FRAME_TYPE_KEY_FRAME = 0x01,
    VIDEO_FRAME_TYPE_INTER_FRAME = 0x02,
    VIDEO_FRAME_TYPE_DISPOSABLE_INTER_FRAME = 0x03,
    VIDEO_FRAME_TYPE_GENERATED_KEY_FRAME = 0x04,
    VIDEO_FRAME_TYPE_VIDEO_INFO_COMMAND_FRAME = 0x05,
}_VIDEO_FRAME_TYPE_E;

typedef enum{
    VIDEO_CODEC_JPEG = 0x01,
    VIDEO_CODEC_H263 = 0x02,
    VIDEO_CODEC_SCREEN_VIDEO = 0x03,
    VIDEO_CODEC_ON2_VP6 = 0x04,
    VIDEO_CODEC_ON2_VP6_WITH_ALPHA = 0x05,
    VIDEO_CODEC_SCREEN_VIDEO_V2 = 0x06,
    VIDEO_CODEC_AVC = 0x07,
}_VIDEO_CODEC_ID_E;

typedef struct {
    _VIDEO_FRAME_TYPE_E FrameType;// 4bit
    _VIDEO_CODEC_ID_E CodecID;// 4bit
}_video_tag_header;

typedef enum {
    AVC_SEQUENCE_HEADER = 0x00,
    AVC_NALU = 0x01,
    AVC_SEQUENCE_END = 0x02,
}_AVCPacketType_E;

//_VIDEO_CODEC_ID_E = 7
typedef struct {
    _AVCPacketType_E AVCPacketType;//1byte
    uint32 CompositionTime;//3byte
}_avc_video_packet_t;

int video_header_get(unsigned char *buf, _video_tag_header *pvideo_header);
int video_avc_header_get(unsigned char *buf, _avc_video_packet_t *pavc_video_header);

typedef enum{
    AMF_DATA_TYPE_NUMBER = 0x00,
    AMF_DATA_TYPE_BOOL= 0x01,
    AMF_DATA_TYPE_STRING= 0x02,
    AMF_DATA_TYPE_OBJECT= 0x03,
    AMF_DATA_TYPE_MovieClip= 0x04,
    AMF_DATA_TYPE_NULL= 0x05,
    AMF_DATA_TYPE_UNDEFINE= 0x06,
    AMF_DATA_TYPE_REFERENCE= 0x07,
    AMF_DATA_TYPE_ECMA_array= 0x08,
    AMF_DATA_TYPE_OBJECT_END= 0x09,
    AMF_DATA_TYPE_ARRAY= 0x0A,
    AMF_DATA_TYPE_DATE= 0x0B,
    AMF_DATA_TYPE_LONG_STRING= 0x0C,
}_amf_type_e;

int AMF_data_parse(unsigned char *buf,uint32 max_len);

typedef struct{
    uint32 StringLength;
    uint8 StringData;
}SCRIPTDATASTRING;


#endif //FLV_FLV_H
