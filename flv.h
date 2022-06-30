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
    uint8 SoundRate;//2 bit 0 5.5KHZ 1 11KHZ 2 22KHZ 3 44KHZ
    uint8 SoundSize;//1 bit 0 snd8bit 1 snd16bit
    uint8 SoundType;//1 bit 0 sndMono 1 sndStereo
}_audio_tag_header;

int audio_header_get(unsigned char *buf, _audio_tag_header *paudio_header);

typedef struct{
    uint8 AACPacketType;//
}_aac_audio_data_t;
int aac_pkg_type_parse(unsigned char *buf,_aac_audio_data_t *paac_audio_data);

//https://blog.csdn.net/jwybobo2007/article/details/9221657
typedef struct{
    uint32 audioObjectType;// = (0x11 & 0xF8) >> 3 <- 取前5bit，结果为2
    uint32 samplingFrequencyIndex;// = ((0x11 & 0x7) << 1) | (0x90 >> 7)<- 取后4bit，结果为3
    uint32 channelConfiguration;// = (0x90 >> 3) & 0x0F <- 取后4bit，结果为2
    uint32 frameLengthFlag;// = (0x90 >> 2) & 0x01<- 取后1bit，结果为0
    uint32 dependsOnCoreCoder;// = (0x90 >> 1) & 0x01 <- 取后1bit，结果为0
    uint32 extensionFlag;// = 0x90 & 0x01 <- 最后1bit，结果始终为0
}_aac_AudioSpecificConfig_t;
int aac_AudioSpecificConfig_parse(unsigned char *buf,_aac_AudioSpecificConfig_t *pAudioSpecificConfig);

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


//aligned(8) class AVCDecoderConfigurationRecord {
//        unsigned int(8) configurationVersion = 1;
//        unsigned int(8) AVCProfileIndication;
//        unsigned int(8) profile_compatibility;
//        unsigned int(8) AVCLevelIndication;
//        bit(6) reserved = ‘111111’b;
//        unsigned int(2) lengthSizeMinusOne;
//        bit(3) reserved = ‘111’b;
//        unsigned int(5) numOfSequenceParameterSets;
//        for (i=0; i< numOfSequenceParameterSets; i++) {
//            unsigned int(16) sequenceParameterSetLength ;
//            bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;
//        }
//        unsigned int(8) numOfPictureParameterSets;
//        for (i=0; i< numOfPictureParameterSets; i++) {
//            unsigned int(16) pictureParameterSetLength;
//            bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
//        }
//        if( profile_idc == 100 || profile_idc == 110 ||
//        profile_idc == 122 || profile_idc == 144 )
//        {
//            bit(6) reserved = ‘111111’b;
//            unsigned int(2) chroma_format;
//            bit(5) reserved = ‘11111’b;
//            unsigned int(3) bit_depth_luma_minus8;
//            bit(5) reserved = ‘11111’b;
//            unsigned int(3) bit_depth_chroma_minus8;
//            unsigned int(8) numOfSequenceParameterSetExt;
//            for (i=0; i< numOfSequenceParameterSetExt; i++) {
//                unsigned int(16) sequenceParameterSetExtLength;
//                bit(8*sequenceParameterSetExtLength) sequenceParameterSetExtNALUnit;
//            }
//        }
//}

typedef struct {
    uint32 data_len;
    uint8 *pdata;
}AVC_sps_pps_unit;

typedef struct{
    uint32 unit_size;
    AVC_sps_pps_unit **unit;
}AVC_sps_pps_pkt;

typedef AVC_sps_pps_pkt AVC_sps_pkt;
typedef AVC_sps_pps_pkt AVC_pps_pkt;

typedef struct{
    uint32 configurationVersion;//1byte
    uint32 AVCProfileIndication;//1byte
    uint32 profile_compatibility;//1byte
    uint32 AVCLevelIndication;//1byte
    //uint8 reserved;//6bit ‘111111’b;
    uint32 lengthSizeMinusOne;//2bit
    //uint8 reserved//3bit = ‘111’b;

    uint32 numOfSequenceParameterSets;//5bit
    AVC_sps_pps_pkt sps_pkt;

    uint32 numOfPictureParameterSets;;//5bit
    AVC_sps_pps_pkt pps_pkt;

}AVCDecoderConfigurationRecord_t;

int AVCDecoderConfigurationRecord_parse(uint8 *buf,uint32 buf_len,AVCDecoderConfigurationRecord_t *pAVCDecoderConfigurationRecord);
int AVCDecoderConfigurationRecord_free(AVCDecoderConfigurationRecord_t *pAVCDecoderConfigurationRecord);

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



typedef struct{
    uint32 syncword;//12
    uint32 id;//1
    uint32 layer;//2
    uint32 protection_absent;//1
    uint32 profile;//2
    uint32 sampling_frequency_index;//4
    uint32 private_bit;//1
    uint32 channel_configuration;//3
    uint32 original_copy;//1
    uint32 home;//1
    uint32 copyright_identification_bit;//1
    uint32 copyright_identification_start;//1
    uint32 aac_frame_length;//13
    uint32 adts_buffer_fullness;//11
    uint32 number_of_raw_data_blocks_in_frame;
}_adts_header_construct_t;

int adts_header_construct_to_buf(_adts_header_construct_t *pconstrcut,_audio_tag_header *paudio_header,uint8 *pbuf);

#endif //FLV_FLV_H
