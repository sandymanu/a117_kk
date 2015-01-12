/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#define LCM_PRINT printf
#define LCM_DBG(fmt, arg...) \
	LCM_PRINT("[LCM-LT4015W-DSI] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)

#else
#include <linux/string.h>

#if defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
	#define LCM_PRINT printf
	#ifndef KERN_INFO
		#define KERN_INFO
	#endif
#else
	#include <linux/kernel.h>
	#include <mach/mt_gpio.h>
	#define LCM_PRINT printk
#endif

#define LCM_DBG
#if  defined(LCM_DBG)
#define LCM_DBG(fmt, arg...) \
	LCM_PRINT("[LCM-lt5002f-DSI] %s (line:%d) :" fmt "\r\n", __func__, __LINE__, ## arg)
#else
#define LCM_DBG(fmt, arg...) do {} while (0)
#endif
#endif //BUILD_LK

#define LCM_DEBUG
#include "lcm_drv.h"

#define LCM_DSI_CMD_MODE									1

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  										(480)
#define FRAME_HEIGHT 										(854)
#define ACTIVE_WIDTH  										(66.86)
#define ACTIVE_HEIGHT 										(120.3)

#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFA   // END OF REGISTERS MARKER

#define LCM_ID_OTM8009A		0x8009

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

//edit by Magnum 2012-10-12  solve screen not update when ESD (+/- 10kv)
#define CHECK_LCM_MODE     

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    								(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 											(lcm_util.udelay(n))
#define MDELAY(n) 											(lcm_util.mdelay(n))
static unsigned int lt5002f_cmp_id(void);
static unsigned int test_cmp_id(void);
static unsigned int check_display_normal(void);

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_V2(cmd,buffer,buffer_size)					lcm_util.dsi_dcs_read_lcm_reg_v2(cmd,buffer,buffer_size)


struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/


	// enable EXTC
	{0x00,	1,	{0x00}},
	{0xff,	3,	{0x80,0x09,0x01}}, 
	{REGFLAG_DELAY, 10, {}},

	
	// shift address
	{0x00,	1,	{0x80}}, 
	// enable Orise mode
     {0xff,	2,	{0x80,0x09}}, 
      

	
        // Column Inversion
	{0x00,	1,	{0x03}}, 
    {0xFF,	1,	{0x01}}, 
     

	
	// pump45 01->X6X6 00->X8X6
	{0x00,	1,	{0x90}}, 
	{0xB3,	1,	{0x02}}, 
	 
	
	//pump1 X2 ratio
	{0x00,	1,	{0x92}}, 
	{0xB3,	1,	{0x45}}, 
	 
	
	//  pump34 CLK Line Rate 
	{0x00,	1,	{0xA6}}, 
	{0xB3,	2,	{0x20,0X01}}, 
	 
	
	//  pump12 clk freq(Normal)
	{0x00,	1,	{0xA3}},  
	{0xc0,	1,	{0x1B}}, 
	 

	  // source pch
	{0x00,	1,	{0xB4}},
	{0xc0,	1,	{0x50}}, 
	 
	
	//  REG-Pump23 AVEE VCL

	{0x00,	1,	{0x81}}, 
	{0xc4,	1,	{0x04}}, 
	 

	{0x00,	1,	{0xB4}}, 
    {0xc0,	1,	{0x10}},   //  50 lie rotation ,10 point rotation edit by Magnum 2012-8-22 
     


	// source bias
	{0x00,	1,	{0x80}}, 
	{0xc5,	1,	{0x03}}, 
	 

	// VDD_18V=1.6V GVDD test on
	{0x00,	1,	{0x82}},
	{0xc5,	1,	{0x03}}, 
	 

	// [7:4]VGH level=12.5V, [3:0]VGL level=-13V  (setting value for worst pattern)
	{0x00,	1,	{0x90}},
	{0xc5,	5,	{0x96,0X2B,0X04,0X7B,0X33}}, 
	 

	  // GVDD 4.625V
	{0x00,	1,	{0x00}}, 
	{0xd8,	1,	{0x70}}, 
	 

	 // NGVDD -4.625V
	{0x00,	1,	{0x01}}, 
	{0xd8,	1,	{0x70}}, 
	 

	  // Frame rate = 70Hz   
	{0x00,	1,	{0x00}}, 
	{0xD9,	1,	{0x2F}}, 
	 

	 // VS, HS, DE for PORTB   
	{0x00,	1,	{0x81}}, 
	{0xc1,	1,	{0x77}},//edit by Magnum 2013-3-26  original: 0x66 
	 

         // VCOM  
//	{0x00,	1,	{0x00}},
//	{0xd9,	1,	{0x3e}}, 
//     

//-----------------------------------------------------------------------------
//           gamma setting
//------------------------------------------------------------------------
//e10x
      
              {0x00,	1,	{0x00}}, 
              {0xe1,	16,	{0x08,0x13,0x19,0x0d,0x06,0x0D,0x0A,0x08,0x05,0x08,0x0E,0x09,0x0f,0x0D,0x07,0x03}}, 
	           

//e20x
         
              {0x00,	1,	{0x00}}, 
              {0xe2,	16,	{0x08,0x13,0x19,0x0D,0X06,0X0D,0x0A,0x08,0x05,0x08,0x0E,0x09,0x0F,0x0D,0x07,0x03}}, 
	           



 //--------------------------------------------------------------------------------
//		initial setting 2 < tcon_goa_wave >
//--------------------------------------------------------------------------------
													
	// CE8x : vst1, vst2, vst3, vst4
	// shift address
	{0x00,	1,	{0x80}}, 
	{0xce,	12,	{0x85,0x01,0x00,0x84,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 


//CEAx : clka1, clka2
        // shift address
	{0x00,	1,	{0xa0}}, 
	{0xce,	14,	{0x18,0x02,0x03,0x59,0x00,0x00,0x00,0x18,0x01,0x03,0x5A,0x00,0x00,0x00}}, 
	 


//CEBx : clka3, clka4
	 // shift address
	{0x00,	1,	{0xb0}},
	{0xce,	14,	{0x18,0x04,0x03,0x5B,0x00,0x00,0x00,0x18,0x03,0x03,0x5C,0x00,0x00,0x00}}, 
	 


//CFCx : 

	{0x00,	1,	{0xc0}}, 
	{0xcf,	10,	{0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x10,0x00}}, 
	 

//CFDx : 
	// shift address
	
	
	{0x00,	1,	{0xd0}}, 
	{0xcf,	1,	{0x00}}, 
	 


//--------------------------------------------------------------------------------
//		initial setting 3 < Panel setting >
//--------------------------------------------------------------------------------


// cbcx
	 // shift address
	{0x00,	1,	{0x80}}, 
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 


// cbdx  
	  // shift address
	{0x00,	1,	{0x90}}, 
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 

	
// cbex
        // shift address
	{0x00,	1,	{0xA0}}, 
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 

	{0x00,	1,	{0xB0}},
	{0xCB,	10,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 

    {0x00,	1,	{0xC0}}, 
	{0xCB,	15,	{0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 
	
    {0x00,	1,	{0xD0}},
	{0xCB,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x04,0x04,0x04,0x04,0x00,0x00,0x00,0x00}}, 
	 

// cccx
	  // shift address
	
    {0x00,	1,	{0xe0}},
	{0xcb,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 

	{0x00,	1,	{0xf0}},
	{0xcb,	10,	{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}}, 
	 

	{0x00,	1,	{0x80}},
	{0xcc,	10,	{0x00,0x26,0x09,0x0b,0x01,0x25,0x00,0x00,0x00,0x00}}, 
	 

	{0x00,	1,	{0x90}},
	{0xcc,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0a,0x0c,0x02}}, 
	 

	{0x00,	1,	{0xa0}},
	{0xcc,	15,	{0x25,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x26,0x0a,0x0c,0x02}}, 
	 

	{0x00,	1,	{0xb0}},
	{0xcc,	10,	{0x00,0x25,0x0a,0x0c,0x02,0x26,0x00,0x00,0x00,0x00}}, 
	 

	{0x00,	1,	{0xc0}},
	{0xcc,	15,	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x09,0x0b,0x01}}, 
	 

	{0x00,	1,	{0xd0}},
	{0xcc,	15,	{0x26,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}}, 
	 

	//open TE  Magnum
	{0x35, 1, {0x00}}, //TE On 

	{0x00, 1, {0x00}},
	{0x44, 2, {0x02,0x01}},
	
  // {0x21, 1, {0x00}},   //Display off for Test Magnum 2012-7-16
  						
	{0x11,1,   {0x00}}, 
	{REGFLAG_DELAY, 120, {}},
	{0x29,1,	{0x00}}, 
    {REGFLAG_DELAY, 200, {}},

	// Note
	// Strongly recommend not to set Sleep out / Display On here. That will cause messed frame to be shown as later the backlight is on.


	// Setting ending by predefined flag
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_set_window[] = {
	{0x2A,	4,	{0x00, 0x00, (FRAME_WIDTH>>8), (FRAME_WIDTH&0xFF)}},
	{0x2B,	4,	{0x00, 0x00, (FRAME_HEIGHT>>8), (FRAME_HEIGHT&0xFF)}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
	{0x11, 1, {0x00}},
	{REGFLAG_DELAY, 150, {}},

    // Display ON
	{0x29, 1, {0x00}},
	{REGFLAG_DELAY, 20, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lcm_compare_id_setting[] = {

	{0x00,	1,	{0x00}},
	{0xff,	3,	{0x80,0x09,0x01}}, 
	{REGFLAG_DELAY, 10, {}},

	{0x00,	1,	{0x80}},
	{0xff,	3,	{0x80,0x09,0x01}}, 
	{REGFLAG_DELAY, 10, {}},

	{0x00,	1,	{0x02}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}


//	array[1]=0x010980ff; 
//	array[2]=0x80001500;
//	array[3]=0x00033902;
//	array[4]=0x010980ff;
};

static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x28, 1, {0x00}},

    // Sleep Mode On
	{0x10, 1, {0x00}},
	{REGFLAG_DELAY, 120, {}},

	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table start[] = {
	{0x00, 1 ,{0x00}}, {0xff, 3  ,{0x80,0x09,0x01}},
	{0x00, 1 ,{0x80}}, {0xff, 2  ,{0x80,0x09}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table turnOnCe[] = {
	{0x00, 1 ,{0x00}}, {0xff, 3  ,{0x80,0x09,0x01}},
	{0x00, 1 ,{0x80}}, {0xff, 2  ,{0x80,0x09}},
	{0x00,1,{0x80}},
	{0xd6,1, {0x08}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table turnOffCe[] = {
	{0x00, 1 ,{0x00}}, {0xff, 3  ,{0x80,0x09,0x01}},
	{0x00, 1 ,{0x80}}, {0xff, 2  ,{0x80,0x09}},
	{0x00,1,{0x80}},
	{0xd6,1, {0x00}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table writeD4[] = {
	{0x00, 1, {0x00}},
	
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
					
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
					
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
					
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
					
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
					
	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},

	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},

	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},

	{0xd4, 40, {	0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40,
					0x00,0x40,0x00,0x40,0x00,0x40,0x00,0x40, 0x00,0x40}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table highCe[] = {
	{0x00, 1, {0x00}},
	{0xd5, 40, {	0x00, 0x60, 0x00, 0x60, 0x00, 0x5f, 0x00, 0x5f, 0x00, 0x5e, 
					0x00, 0x5e, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5c, 
					0x00, 0x5c, 0x00, 0x5b, 0x00, 0x5b, 0x00, 0x5a, 0x00, 0x5a, 
					0x00, 0x5a, 0x00, 0x5b, 0x00, 0x5c, 0x00, 0x5d, 0x00, 0x5d}},
					
	{0xd5, 40, {	0x00, 0x5e, 0x00, 0x5f, 0x00, 0x60, 0x00, 0x61, 0x00, 0x62, 
					0x00, 0x63, 0x00, 0x63, 0x00, 0x64, 0x00, 0x65, 0x00, 0x66,
					0x00, 0x67, 0x00, 0x68, 0x00, 0x69, 0x00, 0x69, 0x00, 0x6a,
					0x00, 0x6b, 0x00, 0x6c, 0x00, 0x6d, 0x00, 0x6e, 0x00, 0x6f}},
							
    {0xd5, 40, {	0x00, 0x6f, 0x00, 0x70, 0x00, 0x71, 0x00, 0x72, 0x00, 0x73, 
    				0x00, 0x74, 0x00, 0x74, 0x00, 0x75, 0x00, 0x76, 0x00, 0x77, 
    				0x00, 0x78, 0x00, 0x78, 0x00, 0x79, 0x00, 0x7a, 0x00, 0x7b, 
    				0x00, 0x7c, 0x00, 0x7d, 0x00, 0x7d, 0x00, 0x7e, 0x00, 0x7f}},
							
	{0xd5, 40, {	0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f}},

    {0xd5, 40, {	0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
    				0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
    				0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
    				0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f}},

	{0xd5, 40, {	0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f}},
							
	{0xd5, 40, {	0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7f,
					0x00, 0x7f, 0x00, 0x7f, 0x00, 0x7e, 0x00, 0x7d, 0x00, 0x7c}},

	{0xd5, 40, {	0x00, 0x7b, 0x00, 0x7a, 0x00, 0x7a, 0x00, 0x79, 0x00, 0x78, 
					0x00, 0x77, 0x00, 0x76, 0x00, 0x76, 0x00, 0x75, 0x00, 0x74,
					0x00, 0x73, 0x00, 0x72, 0x00, 0x71, 0x00, 0x71, 0x00, 0x70,
					0x00, 0x6f, 0x00, 0x6e, 0x00, 0x6d, 0x00, 0x6c, 0x00, 0x6c}},
							
	{0xd5, 40, {	0x00, 0x6b, 0x00, 0x6a, 0x00, 0x69, 0x00, 0x68, 0x00, 0x67, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x65, 0x00, 0x65,
					0x00, 0x64, 0x00, 0x64, 0x00, 0x63, 0x00, 0x63, 0x00, 0x63, 
					0x00, 0x62, 0x00, 0x62, 0x00, 0x61, 0x00, 0x61, 0x00, 0x60}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table middleCe[] = {
	{0x00, 1, {0x00}},
	{0xd5, 40, {	 0x00, 0x5a, 0x00, 0x59, 0x00, 0x59, 0x00, 0x58, 0x00, 0x58, 
				    0x00, 0x57, 0x00, 0x57, 0x00, 0x57, 0x00, 0x56, 0x00, 0x56, 
				    0x00, 0x55, 0x00, 0x55, 0x00, 0x54, 0x00, 0x54, 0x00, 0x54, 
				    0x00, 0x54, 0x00, 0x54, 0x00, 0x55, 0x00, 0x56, 0x00, 0x57}},
					
	{0xd5, 40, {	 0x00, 0x58, 0x00, 0x59, 0x00, 0x5a, 0x00, 0x5a, 0x00, 0x5b, 
				    0x00, 0x5c, 0x00, 0x5d, 0x00, 0x5e, 0x00, 0x5f, 0x00, 0x60, 
				    0x00, 0x60, 0x00, 0x61, 0x00, 0x62, 0x00, 0x62, 0x00, 0x63, 
				    0x00, 0x64, 0x00, 0x64, 0x00, 0x65, 0x00, 0x65, 0x00, 0x66 }},
							
    {0xd5, 40, {	0x00, 0x67, 0x00, 0x67, 0x00, 0x68, 0x00, 0x69, 0x00, 0x69, 
				    0x00, 0x6a, 0x00, 0x6b, 0x00, 0x6b, 0x00, 0x6c, 0x00, 0x6c, 
				    0x00, 0x6d, 0x00, 0x6e, 0x00, 0x6e, 0x00, 0x6f, 0x00, 0x70, 
				    0x00, 0x70, 0x00, 0x71, 0x00, 0x72, 0x00, 0x72, 0x00, 0x73}},
							
	{0xd5, 40, {	0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73}},

    {0xd5, 40, {	0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73}},

	{0xd5, 40, {	0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73 }},
							
	{0xd5, 40, {	0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 0x00, 0x73, 
				    0x00, 0x73, 0x00, 0x73, 0x00, 0x72, 0x00, 0x71, 0x00, 0x71 }},

	{0xd5, 40, {	0x00, 0x70, 0x00, 0x6f, 0x00, 0x6f, 0x00, 0x6e, 0x00, 0x6d, 
				    0x00, 0x6d, 0x00, 0x6c, 0x00, 0x6c, 0x00, 0x6b, 0x00, 0x6a, 
				    0x00, 0x6a, 0x00, 0x69, 0x00, 0x68, 0x00, 0x68, 0x00, 0x67, 
				    0x00, 0x66, 0x00, 0x66, 0x00, 0x65, 0x00, 0x64, 0x00, 0x64 }},
							
	{0xd5, 40, {	0x00, 0x63, 0x00, 0x63, 0x00, 0x62, 0x00, 0x61, 0x00, 0x61, 
				    0x00, 0x60, 0x00, 0x60, 0x00, 0x5f, 0x00, 0x5f, 0x00, 0x5e, 
				    0x00, 0x5e, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5c, 
				    0x00, 0x5c, 0x00, 0x5b, 0x00, 0x5b, 0x00, 0x5a, 0x00, 0x5a}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static struct LCM_setting_table lowCe[] = {
	{0x00, 1, {0x00}},
	{0xd5, 40, {	0x00, 0x53, 0x00, 0x53, 0x00, 0x52, 0x00, 0x52, 0x00, 0x51, 
					0x00, 0x51, 0x00, 0x51, 0x00, 0x50, 0x00, 0x50, 0x00, 0x4f, 
					0x00, 0x4f, 0x00, 0x4f, 0x00, 0x4e, 0x00, 0x4e, 0x00, 0x4d, 
					0x00, 0x4d, 0x00, 0x4e, 0x00, 0x4f, 0x00, 0x50, 0x00, 0x51}},
					
	{0xd5, 40, {	0x00, 0x51, 0x00, 0x52, 0x00, 0x53, 0x00, 0x54, 0x00, 0x55, 
					0x00, 0x56, 0x00, 0x57, 0x00, 0x57, 0x00, 0x58, 0x00, 0x59, 
					0x00, 0x5a, 0x00, 0x5a, 0x00, 0x5b, 0x00, 0x5b, 0x00, 0x5c, 
					0x00, 0x5c, 0x00, 0x5c, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5e}},
							
    {0xd5, 40, {	0x00, 0x5e, 0x00, 0x5f, 0x00, 0x5f, 0x00, 0x5f, 0x00, 0x60, 
					0x00, 0x60, 0x00, 0x61, 0x00, 0x61, 0x00, 0x61, 0x00, 0x62, 
					0x00, 0x62, 0x00, 0x63, 0x00, 0x63, 0x00, 0x64, 0x00, 0x64, 
					0x00, 0x64, 0x00, 0x65, 0x00, 0x65, 0x00, 0x66, 0x00, 0x66}},
							
	{0xd5, 40, {	0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66}},

    {0xd5, 40, {	0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66}},

	{0xd5, 40, {	0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66}},
							
	{0xd5, 40, {	0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 
					0x00, 0x66, 0x00, 0x66, 0x00, 0x66, 0x00, 0x65, 0x00, 0x65}},

	{0xd5, 40, {	0x00, 0x64, 0x00, 0x64, 0x00, 0x63, 0x00, 0x63, 0x00, 0x63, 
					0x00, 0x62, 0x00, 0x62, 0x00, 0x61, 0x00, 0x61, 0x00, 0x60, 
					0x00, 0x60, 0x00, 0x60, 0x00, 0x5f, 0x00, 0x5f, 0x00, 0x5e, 
					0x00, 0x5e, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5d, 0x00, 0x5c}},
							
	{0xd5, 40, {	0x00, 0x5c, 0x00, 0x5b, 0x00, 0x5b, 0x00, 0x5a, 0x00, 0x5a, 
					0x00, 0x5a, 0x00, 0x59, 0x00, 0x59, 0x00, 0x58, 0x00, 0x58, 
					0x00, 0x57, 0x00, 0x57, 0x00, 0x57, 0x00, 0x56, 0x00, 0x56, 
					0x00, 0x55, 0x00, 0x55, 0x00, 0x54, 0x00, 0x54, 0x00, 0x54}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for(i = 0; i < count; i++) 
	{
		unsigned cmd;
		cmd = table[i].cmd;

		switch (cmd) {

		case REGFLAG_DELAY :
			MDELAY(table[i].count);
			break;

		case REGFLAG_END_OF_TABLE :
			break;

		default:
			dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
		}
    	}
	
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));
	
	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;
	//params->active_width= ACTIVE_WIDTH;
	//params->active_height= ACTIVE_HEIGHT;



		// enable tearing-free 
     params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_ONLY; 
       //params->dbi.te_mode                 = LCM_DBI_TE_MODE_VSYNC_OR_HSYNC; 
    params->dbi.te_edge_polarity        = LCM_POLARITY_FALLING; 

		
#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_TWO_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
//Ivan	params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.intermediat_buffer_num = 0;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
		params->dsi.vertical_active_line=FRAME_HEIGHT;
/*
		params->dsi.word_count=480*3;	

		params->dsi.vertical_sync_active=2;
		params->dsi.vertical_backporch=2;
		params->dsi.vertical_frontporch=2;
	
		params->dsi.line_byte=2180;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=206;
		params->dsi.horizontal_frontporch_byte=206;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=200;
		params->dsi.horizontal_frontporch_word_count=200;
*/
		// Bit rate calculation
//		params->dsi.pll_div1=1;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
//		params->dsi.pll_div2=1;		// div2=0,1,2,3;div1_real=1,2,4,4	
//		params->dsi.fbk_div =23;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	

		// the  biggest enum int LCM_DSI_PLL_CLOCK is 31 ,bigger than 31 is bad  
		params->dsi.PLL_CLOCK = LCM_DSI_6589_PLL_CLOCK_312 ;        

}


static void lcm_init(void)
{
	LCM_DBG();    
	SET_RESET_PIN(1);
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(100);

	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);

	push_table(turnOnCe, sizeof(turnOnCe) / sizeof(struct LCM_setting_table), 1);   
	push_table(writeD4, sizeof(writeD4) / sizeof(struct LCM_setting_table), 1);  

    //    if(0 == strncmp(TINNO_PROJECT_NAME, "S9070", 5))
   //     push_table(lcm_scanning_direction_setting, sizeof(lcm_scanning_direction_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{
	LCM_DBG();
    
	push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_resume(void)
{

	LCM_DBG();
//	lcm_init();
#if 1
    if(test_cmp_id())
		push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
	else
		lcm_init();  
#endif
}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
   // LCM_DBG("UPDATE-UPDATE-UPDATE");
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(&data_array, 3, 1);
	//data_array[0]= 0x00290508;                   // should delete it ...edit 
	// by Magnum
	//dsi_set_cmdq(&data_array, 1, 1);	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(&data_array, 1, 0);

}

static unsigned int lt5002f_cmp_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  


//reset before read id
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(25);
	SET_RESET_PIN(1);
	MDELAY(50);

//
 /*   array[0]=0x00043902;
	array[1]=0x010980ff; 
	array[2]=0x80001500;
	array[3]=0x00033902;
	array[4]=0x010980ff;
	dsi_set_cmdq(array, 5, 1);
	MDELAY(10);

	

	array[0] = 0x02001500; //
	dsi_set_cmdq(array, 1, 1);  */
    push_table(lcm_compare_id_setting, sizeof(lcm_compare_id_setting) / sizeof(struct LCM_setting_table), 1);
	array[0] = 0x00023700;// set return byte number
	dsi_set_cmdq(array, 1, 1);

   
 //   MDELAY(10);
	
	read_reg_V2(0xD2, &buffer, 2);

	id = buffer[0]<<8 |buffer[1]; 

	LCM_DBG("[Magnum] -- otm8009a 0x%x , 0x%x , 0x%x \n",buffer[0],buffer[1],id);
		
	return (id == LCM_ID_OTM8009A)?1:0;

}

static unsigned int test_cmp_id(void)
{
	unsigned int id=0;
	unsigned char buffer[2];
	unsigned int array[16];  


//reset before read id
    array[0]=0x00043902;
	array[1]=0x010980ff; 
	dsi_set_cmdq(array, 2, 1);
	
	array[0]=0x80001500;
	dsi_set_cmdq(array, 1, 1);
	
	array[0]=0x00033902;
	array[1]=0x010980ff;
	dsi_set_cmdq(array, 2, 1);
	MDELAY(10);

	array[0] = 0x02001500;
	dsi_set_cmdq(array, 1, 1);


	array[0] = 0x00023700;// set return byte number
	dsi_set_cmdq(array, 1, 1);

	read_reg_V2(0xD2, &buffer, 2);

	id = buffer[0]<<8 |buffer[1]; 

	LCM_DBG("[LSQ] -- otm8009a 0x%x , 0x%x , 0x%x \n",buffer[0],buffer[1],id);
		
	return (id == LCM_ID_OTM8009A)?1:0;

}

static unsigned int lcm_esd_check(void)
{
    unsigned int ret=0;
    ret = check_display_normal();//test_cmp_id();
    if (ret)
		return 0;
    return 1;
	
}


static unsigned int check_display_normal(void)
{
    unsigned int normal=0;
    unsigned char buffer1[1];
	unsigned char buffer2[1];
	unsigned char buffer3[1];
	unsigned char buffer4[1];
	unsigned int array[16]; 
	array[0] = 0x00013700;// set return byte number
	dsi_set_cmdq(array, 1, 1);

	read_reg_V2(0x0A, &buffer1, 1);
	read_reg_V2(0x0B, &buffer2, 1);
	read_reg_V2(0x0C, &buffer3, 1);
	read_reg_V2(0x0D, &buffer4, 1);
//	normal = buffer[0];
	LCM_DBG("[LSQ] --test ic normal == 0x%x , 0x%x , 0x%x , 0x%x\n",buffer1[0],buffer2[0],buffer3[0],buffer4[0]);
    if(buffer1[0] == 0x9c && buffer2[0] == 0x0 && buffer3[0] == 0x7 && buffer4[0] == 0x0)
	 	return 1;
	else
		return 0;
}

//edit by Magnum 2012-12-6 ajust lcd  for android settings.
static bool enableCE(char* ver)
{
    char high = 'h';    // can not be a-f.
	char middle = 'm';
	char low = 'l';
	char close = 'g';
	char temp = ver[0];
	if(temp == high){
		LCM_DBG("Accept High CE");  
		push_table(turnOffCe, sizeof(turnOffCe) / sizeof(struct LCM_setting_table), 1);  
		push_table(turnOnCe, sizeof(turnOnCe) / sizeof(struct LCM_setting_table), 1);   
		push_table(writeD4, sizeof(writeD4) / sizeof(struct LCM_setting_table), 1);
		push_table(highCe, sizeof(highCe) / sizeof(struct LCM_setting_table), 1);  
		
	}
	else if(temp == middle ){
		LCM_DBG("Accept Middle CE");
		push_table(turnOffCe, sizeof(turnOffCe) / sizeof(struct LCM_setting_table), 1);  
		push_table(turnOnCe, sizeof(turnOnCe) / sizeof(struct LCM_setting_table), 1);   
		push_table(writeD4, sizeof(writeD4) / sizeof(struct LCM_setting_table), 1);   
		push_table(middleCe, sizeof(middleCe) / sizeof(struct LCM_setting_table), 1);  
	}
	else if(temp == low ){
		LCM_DBG("Accept Middle CE");
		push_table(turnOffCe, sizeof(turnOffCe) / sizeof(struct LCM_setting_table), 1);  
		push_table(turnOnCe, sizeof(turnOnCe) / sizeof(struct LCM_setting_table), 1);   
		push_table(writeD4, sizeof(writeD4) / sizeof(struct LCM_setting_table), 1);   
		push_table(lowCe, sizeof(lowCe) / sizeof(struct LCM_setting_table), 1);  
	}
	else if(temp == close){
		LCM_DBG("Accept Low CE");
		push_table(turnOffCe, sizeof(turnOffCe) / sizeof(struct LCM_setting_table), 1);  
	}
	else{
		LCM_DBG("Debug LCM");
		return false;
	}
	return true;
}

#ifdef LCM_DEBUG
char * magnum_strsep(char **s, const char *ct)
{
	char *sbegin = *s, *end;

	if (sbegin == NULL)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;

	return sbegin;
}
static struct LCM_setting_table  lcm_debug_params[100];
static int register_count = 0;
static char  tempbuf[100][500];

//just for register and, register length is 2
static int ver_2_num(char* ver)
{
	LCM_DBG("ver ====== %s",ver);
    unsigned long var=0;
    unsigned long t;
    int len = strlen(ver);
//	LCM_DBG("data length %d",len);
    if (var > 8) //\u93c8\u20ac\u95c0?\u6d63?
 	 return -1;
	 int i = 0;
	//for (i; i!=2; i++)
	for (i; ver[i] !='\0'; i++)
	 {
	     if(i==2)
		 	break;
	 	 if (ver[i]>='a' && ver[i] <='f')
		   t = ver[i]-87;
	 	 else if (ver[i]>='A' && ver[i] <='F')
		   t = ver[i]-55;
		 else if(ver[i] < 47)
		  	 continue;
	 	 else
	 	 t = ver[i]-48;
//		 LCM_DBG("t ====== %x",t);
	 	 var<<=4;
	  	 var|=t;
     }
	return var; 
}

static void GetDebugInfo(char * cmd, char * data,int reg_count)
{
	unsigned tempcmd = ver_2_num(cmd);
	int data_count = 0;
	lcm_debug_params[reg_count].cmd = tempcmd;
	char *p = NULL;
	char delim2[] = ",";
	//p = magnum_strsep(&data,delim2);   // split to  para_list
	LCM_DBG("data  = %s",data);
	 unsigned int datatemp;
	 while((p=magnum_strsep(&data,delim2))){
	 	     datatemp = ver_2_num(p);
			 LCM_DBG("data temp = %x",datatemp);
	        lcm_debug_params[reg_count].para_list[data_count] = datatemp;
        	data_count++;	
        }
      
    lcm_debug_params[reg_count].count = data_count;
   	LCM_DBG("[Magnum] GetDebugInfo...cmd=%x, count = %d \n",lcm_debug_params[reg_count].cmd,lcm_debug_params[reg_count].count);
	int j = 0;
	while(j!= data_count){
		LCM_DBG("para_list=%x\n",lcm_debug_params[reg_count].para_list[j]);
		j++;
	}
	LCM_DBG("\n");
	
}
static void handle_lcm_debug_buff(char * buf)
{

	LCM_DBG("[MAGNUM]get lcm debug buffer == %s \n",buf);
	int length = strlen(buf);
	if(length <= 0){
		LCM_DBG("RETURN .............. \n");
		return;
		}
	char delim[]="+";
    char *p=NULL;
    char *p1=NULL;
    char * pCmd = NULL;
	char * pData = NULL;
	char delim1[]=";";
	
	//p = magnum_strsep(&buf,delim);   // split to  LCM_setting_table
//	 while(NULL != (p=magnum_strsep(&buf,delim))){
	while(p=magnum_strsep(&buf,delim)){
        	strcpy(tempbuf[register_count],p);
        //	tempbuf[register_count] = p;
        	LCM_DBG("tempbuf == %s\n",tempbuf[register_count]);
        	register_count++;	
        }	
	LCM_DBG("register_count ==[%d]\n",register_count);
    int i = 0;
    for( i;i<register_count;i++)
    {
		p1 = tempbuf[i];
		pCmd = magnum_strsep(&p1,delim1); // split to cmd & data;
		LCM_DBG("lcm_debug_params[%d]: ",i);
		LCM_DBG("cmd == %s  && ",pCmd);
		pData=magnum_strsep(&p1,delim1);
		LCM_DBG("data == %s && length ==%d\n",pData,strlen(pData));
		GetDebugInfo(pCmd,pData,i);
    }
//	lcm_debug_params[register_count].cmd = 255;
 //   lcm_debug_params[register_count].count = 0;
   // lcm_debug_params[register_count].count
   i = 0;
 //  while(++i < (register_count +1))
   while(++i < (register_count ))
   {
   		LCM_DBG("lcm_debug_params[%d]: ",i);
		LCM_DBG("cmd == %x  && ",lcm_debug_params[i].cmd);
		LCM_DBG("count == %d  && ",lcm_debug_params[i].count);
		char * para = lcm_debug_params[i].para_list;
		LCM_DBG("data == ");
		int j = 0;
		while(j!= lcm_debug_params[i].count){
			LCM_DBG(" %x ,",lcm_debug_params[i].para_list[j]);
			j++;
		}
		LCM_DBG("\n");
   }
	
}

static void lcm_debug(char * buf)
{
	//edit by Magnum 2012-11-1
	//static struct LCM_setting_table  lcm_debug_params[];
	     //  lcm_debug[0] = {cmd ,count,};
	//     lcm_debug_params[0].cmd = cmd;
	//	 lcm_debug_params[0].count = count;
	//	 strcpy(lcm_debug_params[0].para_list,para_list);
	//char * p = strdup(buf);
	LCM_DBG("[Magnum] lcm_debug %s\n",buf);
//	if(enableCE(buf))
	//	return;
	handle_lcm_debug_buff(buf);
	
	push_table(lcm_debug_params, register_count, 1);
	register_count = 0;
	memset(lcm_debug_params,0,sizeof(lcm_debug_params) / sizeof(struct LCM_setting_table));
	LCM_DBG("[Magnum] lcm_debug %x\n",lcm_debug_params[0].cmd);
}
#endif

// ---------------------------------------------------------------------------
//  Get LCM Driver Hooks
// ---------------------------------------------------------------------------
LCM_DRIVER lt5002f_dsi_lcm_drv = 
{
    .name			= "otm8009a_fwvga_dsi_cmd_boe",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
#if (LCM_DSI_CMD_MODE)
	.update         = lcm_update,
#endif
#ifdef CHECK_LCM_MODE
	.esd_check   = lcm_esd_check,
	.esd_recover   = lcm_init,
#endif
#ifdef LCM_DEBUG
	//.m_debug	        = lcm_debug,
#endif
	.compare_id		= lt5002f_cmp_id,
};
