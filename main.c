#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "flv.h"

int file_create(FILE **ppfile,uint8 *pfile_name)
{
    if((*ppfile) == NULL)
    {
        (*ppfile) = fopen(pfile_name/*"video.h264"*/,"wb+");
    }
    return 0;
}
int video_file_create(FILE **pfile)
{
    return file_create(pfile,"video.h264");
}
int audio_file_create(FILE **pfile)
{
    return file_create(pfile,"audio.aac");
}
#define FLV_FILE "D:\\\\flv\\461480359_nb2-1-120.flv"
//https://blog.csdn.net/weixin_43937576/article/details/111033402
//https://blog.csdn.net/abcd1f2/article/details/82189186
int main() {

    FILE *paudio_file = NULL;
    FILE *pvideo_file = NULL;
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

    static AVCDecoderConfigurationRecord_t AVCDecoderConfigurationRecord = {0};

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
                if(audio_tag_header.SoundFormat == 10)
                {
                    _aac_audio_data_t aac_audio_data = {0};
                    aac_pkg_type_parse(&ptag_data[1],&aac_audio_data);

                    static _aac_AudioSpecificConfig_t AudioSpecificConfig = {0};
                    if(aac_audio_data.AACPacketType == 0)
                    {
                        aac_AudioSpecificConfig_parse(&ptag_data[2],&AudioSpecificConfig);
                        continue;
                    }
//                    if(paudio_file == NULL)
//                    {
//                        paudio_file = fopen("audio.aac","wb+");
//                    }
                    audio_file_create(&paudio_file);
                    if(paudio_file)
                    {
                        //https://blog.csdn.net/weixin_40732273/article/details/109458777
                        //https://blog.csdn.net/occupy8/article/details/102490678
                        //https://blog.csdn.net/jay100500/article/details/52955232
                        _adts_header_construct_t adts_header_construct={0};

                        adts_header_construct.syncword = 0xfff;//12
                        adts_header_construct.id = 0x00;//1
                        adts_header_construct.layer = 0x00;//2
                        adts_header_construct.protection_absent = 1;//1 //无crc
                        adts_header_construct.profile = 1;//2
                        adts_header_construct.sampling_frequency_index = AudioSpecificConfig.samplingFrequencyIndex;//4
                        adts_header_construct.private_bit = 0;//1
                        adts_header_construct.channel_configuration = AudioSpecificConfig.channelConfiguration;//3
                        adts_header_construct.original_copy = 0;//1
                        adts_header_construct.home = 0;//1
                        adts_header_construct.copyright_identification_bit = 0;//1
                        adts_header_construct.copyright_identification_start = 0;//1
                        adts_header_construct.aac_frame_length = (data_size - 2) + (adts_header_construct.protection_absent == 0 ? 9 : 7);//13
                        adts_header_construct.adts_buffer_fullness = 0x7FF;//11
                        adts_header_construct.number_of_raw_data_blocks_in_frame = 0;

                        char adts[7]={0};
                        adts_header_construct_to_buf(&adts_header_construct,NULL,adts);

                        fwrite(adts,1,sizeof(adts),paudio_file);
                        fwrite(&ptag_data[2],1,data_size-2,paudio_file);
                    }
                }
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
                if(avc_video_header.AVCPacketType == 1)
                {
                    video_file_create(&pvideo_file);
                    if(pvideo_file)
                    {
//                        https://blog.csdn.net/sunxiaopengsun/article/details/80824506?utm_medium=distribute.pc_relevant.none-task-blog-2~default~baidujs_title~default-0-80824506-blog-80723959.pc_relevant_multi_platform_whitelistv1&spm=1001.2101.3001.4242.1&utm_relevant_index=3
                        uint32 nalu_len = 0;
                        uint8 *pnalu = &ptag_data[5];
                        do{
                            uint8 h264_nalu_header[8] = {0};
                            uint32 h264_nalu_header_len = 0;
                            uint32 nalu_type = 0;
                            nalu_len = (pnalu[0]<<24)|(pnalu[1]<<16)|(pnalu[2]<<8)|(pnalu[3]);
                            nalu_type = pnalu[4];
                            if(0x65 == nalu_type)
                            {
                                {

                                    h264_nalu_header[3] = 0x01;
                                    h264_nalu_header_len = 4;
                                    fwrite(h264_nalu_header,1,h264_nalu_header_len ,pvideo_file);
                                    h264_nalu_header[3] = 0x00;

                                    int index = 0;
                                    for(index = 0;index<AVCDecoderConfigurationRecord.sps_pkt.unit_size;index++)
                                    {
                                        AVC_sps_pps_unit *puint = AVCDecoderConfigurationRecord.sps_pkt.unit[index];
                                        fwrite(puint->pdata,1,puint->data_len ,pvideo_file);
                                    }

                                    h264_nalu_header[3] = 0x01;
                                    h264_nalu_header_len = 4;
                                    fwrite(h264_nalu_header,1,h264_nalu_header_len ,pvideo_file);
        //                            h264_nalu_header[3] = 0x00;

                                    for(index = 0;index<AVCDecoderConfigurationRecord.pps_pkt.unit_size;index++)
                                    {
                                        AVC_sps_pps_unit *puint = AVCDecoderConfigurationRecord.pps_pkt.unit[index];
                                        fwrite(puint->pdata,1,puint->data_len ,pvideo_file);
                                    }
                                }
                            }


                            if(h264_nalu_header[2] || h264_nalu_header[3])
                            {
                                h264_nalu_header[3] = 0x00;
                                h264_nalu_header[2] = 0x01;
                                h264_nalu_header_len = 3;
                            }
                            else
                            {
                                h264_nalu_header[3] = 0x01;
                                h264_nalu_header_len = 4;
                            }
                            fwrite(h264_nalu_header,1,h264_nalu_header_len ,pvideo_file);


                            fwrite(&pnalu[4],1,nalu_len,pvideo_file);
                            pnalu += 4 + nalu_len;
                        } while (pnalu<&ptag_data[data_size-1]);
                    }
                }
                else
                {
                    if(AVCDecoderConfigurationRecord.sps_pkt.unit == NULL && AVCDecoderConfigurationRecord.pps_pkt.unit == NULL)
                    {
                        if(AVCDecoderConfigurationRecord_parse(&ptag_data[5],data_size-5,&AVCDecoderConfigurationRecord) == 0)
                        {
                        }
                    }
                }
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

    if(paudio_file)
    {
        fclose(paudio_file);
        paudio_file = NULL;
    }

    if(pvideo_file)
    {
        fclose(pvideo_file);
        pvideo_file = NULL;
    }
    AVCDecoderConfigurationRecord_free(&AVCDecoderConfigurationRecord);
    return 0;
}
