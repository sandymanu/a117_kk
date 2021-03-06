#ifndef __M4U_PORT_H__
#define __M4U_PORT_H__

//====================================
// about portid
//====================================
#define M4U_LARB0_PORTn(n)      ((n)+0)
#define M4U_LARB1_PORTn(n)      ((n)+10)
#define M4U_LARB2_PORTn(n)      ((n)+17)
#define M4U_LARB3_PORTn(n)      ((n)+29)
#define M4U_LARB4_PORTn(n)      ((n)+43)
#define M4U_LARB5_PORTn(n)      ((n)+53)

typedef enum M4U_PORT_ID
{
    M4U_PORT_VENC_RCPU             =  M4U_LARB0_PORTn(0)   ,
    M4U_PORT_VENC_REF_LUMA         =  M4U_LARB0_PORTn(1)   ,
    M4U_PORT_VENC_REF_CHROMA       =  M4U_LARB0_PORTn(2)   ,
    M4U_PORT_VENC_DB_READ          =  M4U_LARB0_PORTn(3)   ,
    M4U_PORT_VENC_DB_WRITE         =  M4U_LARB0_PORTn(4)   ,
    M4U_PORT_VENC_CUR_LUMA         =  M4U_LARB0_PORTn(5)   ,
    M4U_PORT_VENC_CUR_CHROMA       =  M4U_LARB0_PORTn(6)   ,
    M4U_PORT_VENC_RD_COMV          =  M4U_LARB0_PORTn(7)   ,
    M4U_PORT_VENC_SV_COMV          =  M4U_LARB0_PORTn(8)   ,
    M4U_PORT_VENC_BSDMA            =  M4U_LARB0_PORTn(9)   ,
                                                           
    M4U_PORT_HW_VDEC_MC_EXT        =  M4U_LARB1_PORTn(0)   ,
    M4U_PORT_HW_VDEC_PP_EXT        =  M4U_LARB1_PORTn(1)   ,
    M4U_PORT_HW_VDEC_AVC_MV_EXT    =  M4U_LARB1_PORTn(2)   ,
    M4U_PORT_HW_VDEC_PRED_RD_EXT   =  M4U_LARB1_PORTn(3)   ,
    M4U_PORT_HW_VDEC_PRED_WR_EXT   =  M4U_LARB1_PORTn(4)   ,
    M4U_PORT_HW_VDEC_VLD_EXT       =  M4U_LARB1_PORTn(5)   ,
    M4U_PORT_HW_VDEC_VLD2_EXT      =  M4U_LARB1_PORTn(6)   ,
                                                           
    M4U_PORT_ROT_EXT               =  M4U_LARB2_PORTn(0)   ,
    M4U_PORT_OVL_CH0               =  M4U_LARB2_PORTn(1)   ,
    M4U_PORT_OVL_CH1               =  M4U_LARB2_PORTn(2)   ,
    M4U_PORT_OVL_CH2               =  M4U_LARB2_PORTn(3)   ,
    M4U_PORT_OVL_CH3               =  M4U_LARB2_PORTn(4)   ,
    M4U_PORT_WDMA0                 =  M4U_LARB2_PORTn(5)   ,
    M4U_PORT_WDMA1                 =  M4U_LARB2_PORTn(6)   ,
    M4U_PORT_RDMA0                 =  M4U_LARB2_PORTn(7)   ,
    M4U_PORT_RDMA1                 =  M4U_LARB2_PORTn(8)   ,
    M4U_PORT_CMDQ                  =  M4U_LARB2_PORTn(9)   ,
    M4U_PORT_DBI                   =  M4U_LARB2_PORTn(10)  ,
    M4U_PORT_G2D                   =  M4U_LARB2_PORTn(11)  ,
                                                           
    M4U_PORT_JPGDEC_WDMA           =  M4U_LARB3_PORTn(0)   ,
    M4U_PORT_JPGENC_RDMA           =  M4U_LARB3_PORTn(1)   ,
    M4U_PORT_VIPI                  =  M4U_LARB3_PORTn(2)   ,
    M4U_PORT_IMGI                  =  M4U_LARB3_PORTn(3)   ,
    M4U_PORT_DISPO                 =  M4U_LARB3_PORTn(4)   ,
    M4U_PORT_DISPCO                =  M4U_LARB3_PORTn(5)   ,
    M4U_PORT_DISPVO                =  M4U_LARB3_PORTn(6)   ,
    M4U_PORT_VIDO                  =  M4U_LARB3_PORTn(7)   ,
    M4U_PORT_VIDCO                 =  M4U_LARB3_PORTn(8)   ,
    M4U_PORT_VIDVO                 =  M4U_LARB3_PORTn(9)   ,
    M4U_PORT_VIP2I                 =  M4U_LARB3_PORTn(10)  ,
    M4U_PORT_GDMA_SMI_WR           =  M4U_LARB3_PORTn(11)  ,
    M4U_PORT_JPGDEC_BSDMA          =  M4U_LARB3_PORTn(12)  ,
    M4U_PORT_JPGENC_BSDMA          =  M4U_LARB3_PORTn(13)  ,
                                                           
    M4U_PORT_GDMA_SMI_RD           =  M4U_LARB4_PORTn(0)   ,
    M4U_PORT_IMGCI                 =  M4U_LARB4_PORTn(1)   ,
    M4U_PORT_IMGO                  =  M4U_LARB4_PORTn(2)   ,
    M4U_PORT_IMG2O                 =  M4U_LARB4_PORTn(3)   ,
    M4U_PORT_LSCI                  =  M4U_LARB4_PORTn(4)   ,
    M4U_PORT_FLKI                  =  M4U_LARB4_PORTn(5)   ,
    M4U_PORT_LCEI                  =  M4U_LARB4_PORTn(6)   ,
    M4U_PORT_LCSO                  =  M4U_LARB4_PORTn(7)   ,
    M4U_PORT_ESFKO                 =  M4U_LARB4_PORTn(8)   ,
    M4U_PORT_AAO                   =  M4U_LARB4_PORTn(9)   ,
                                                           
    M4U_PORT_AUDIO                 =  M4U_LARB5_PORTn(0)   ,

    M4U_PORT_NUM,

    M4U_PORT_UNKNOWN         = 1000

};



typedef enum M4U_MODULE_ID
{
    M4U_CLNTMOD_VENC    = 0,	//0
                             
    M4U_CLNTMOD_VDEC       ,
                             
    M4U_CLNTMOD_ROT        ,
    M4U_CLNTMOD_OVL        ,
    M4U_CLNTMOD_WDMA       ,
    M4U_CLNTMOD_RDMA       ,
    M4U_CLNTMOD_CMDQ       ,
    M4U_CLNTMOD_DBI        ,
    M4U_CLNTMOD_G2D        ,
                                 
    M4U_CLNTMOD_JPGDEC     ,
    M4U_CLNTMOD_JPGENC     ,
    M4U_CLNTMOD_VIP        ,
    M4U_CLNTMOD_DISP       ,
    M4U_CLNTMOD_VID        ,
    M4U_CLNTMOD_GDMA       ,
                           
    M4U_CLNTMOD_IMG        ,
    M4U_CLNTMOD_LSCI       ,
    M4U_CLNTMOD_FLKI       ,
    M4U_CLNTMOD_LCEI       ,
    M4U_CLNTMOD_LCSO       ,
    M4U_CLNTMOD_ESFKO      ,
    M4U_CLNTMOD_AAO        ,
                           
    M4U_CLNTMOD_AUDIO      ,
    
    M4U_CLNTMOD_LCDC_UI,
    M4U_CLNTMOD_UNKNOWN,   

    M4U_CLNTMOD_MAX,
};

#endif

