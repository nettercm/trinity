
#ifdef WIN32
//#pragma warning( push )
//#pragma warning( disable : C4103 )
//#pragma warning(disable:C4244)
//#define usb_printf(what) 
#define usb_printf printf
#define pgm_get_far_address(what) 0
#endif

#include "config.h"

#ifndef WIN32
#include <avr/pgmspace.h>
#include "debug.h"
#endif

#ifdef SVP_ON_WIN32
#include <avr/pgmspace.h>
#include "debug.h"
#endif


#ifdef WIN32

#define GRP(grp, name) { grp, 0 , 0, name, 0 },
#define U08(grp, id, name, min, max, value) { grp, id , U08_VALUE, name, value },
#define S08(grp, id, name, min, max, value) { grp, id , S08_VALUE, name, value },
#define U16(grp, id, name, min, max, value) { grp, id , U16_VALUE, name, value },
#define S16(grp, id, name, min, max, value) { grp, id , S16_VALUE, name, value },
#define U32(grp, id, name, min, max, value) { grp, id , U32_VALUE, name, value },
#define S32(grp, id, name, min, max, value) { grp, id , S32_VALUE, name, value },
#define FLT(grp, id, name, min, max, value) { grp, id , FLT_VALUE, name, value },

t_config_flash config[MAX_CFG_ITEMS] =
{
	//{0,0,0,"THE START",0},
	#include "parameters.h"
	{255,255,255,"THE END",0}
};


t_config_flash config_flash[MAX_CFG_ITEMS] =
{
	//{0,0,0,"THE START",0},
	#include "parameters.h"
	{255,255,255,"THE END",0}
};

//workaround to capture the float values
t_config_flash_float_only config_float_only[MAX_CFG_ITEMS] =
{
	//{0,0,0,"THE START",0},
	#include "parameters.h"
	{255,255,255,"THE END", 0}
};

#else

#define GRP(grp, name) { grp, 0 , 0 , name, .v.u32  = 0 },
#define FLT(grp, id, name, min, max, value) { grp, id , FLT_VALUE , name, .v.f  = value },
#define S32(grp, id, name, min, max, value) { grp, id , S32_VALUE , name, .v.s32= value },
#define U32(grp, id, name, min, max, value) { grp, id , U32_VALUE , name, .v.u32= value },
#define S16(grp, id, name, min, max, value) { grp, id , S16_VALUE , name, .v.s16= value },
#define U16(grp, id, name, min, max, value) { grp, id , U16_VALUE , name, .v.u16= value },
#define U08(grp, id, name, min, max, value) { grp, id , U08_VALUE , name, .v.u08= value },
#define S08(grp, id, name, min, max, value) { grp, id , S08_VALUE , name, .v.s08= value },

const t_config_flash config_flash[MAX_CFG_ITEMS] PROGMEM =
{
	//{0,0,0,"THE START",0},
	#include "parameters.h"
	{255,255,255,"THE END",0}
};


#undef U08
#undef S08
#undef U16
#undef S16
#undef U32
#undef S32
#undef FLT
#undef GRP

#define GRP(grp, name) { grp, 0 , .v.u32  = 0 },
#define FLT(grp, id, name, min, max, value) { grp, id , .v.f  = value },
#define U08(grp, id, name, min, max, value) { grp, id , .v.u08= value },
#define S08(grp, id, name, min, max, value) { grp, id , .v.s08= value },
#define U16(grp, id, name, min, max, value) { grp, id , .v.u16= value },
#define S16(grp, id, name, min, max, value) { grp, id , .v.s16= value },
#define U32(grp, id, name, min, max, value) { grp, id , .v.u32= value },
#define S32(grp, id, name, min, max, value) { grp, id , .v.s32= value },

t_config config[MAX_CFG_ITEMS]; /* =
{
	//{0,0,-1},
	#include "parameters.h"
	{255,255,-1}
};
*/

/*
				Program Memory Usage 	:	46588 bytes   35.5 % Full
				Data Memory Usage 		:	4093 bytes   25.0 % Full
*/


#endif

void cfg_init(void)
{
	int i;
	
	for(i=0;i<MAX_CFG_ITEMS;i++)
	{
#ifdef WIN32
		if(config_float_only[i].type == FLT_VALUE)
		{
			config[i].v.f		= config_float_only[i].f;
			config_flash[i].v.f = config_float_only[i].f;
		}
#else
		config[i].grp =   pgm_read_byte(&(config_flash[i].grp));
		config[i].id =    pgm_read_byte(&(config_flash[i].id));
		config[i].v.u32 = pgm_read_dword(&(config_flash[i].v));
#endif
	}
}


void cfg_test(void)
{
	usb_printf("addr of config:       0x%08x\n",config);
	usb_printf("addr of config_flash: 0x%08x\n",pgm_get_far_address(config_flash));
	usb_printf("%d\n", config[0].v.s16);
	
}


void cfg_set_value_by_grp_id(uint8 grp, uint8 id, t_config_value v)
{
	u08 i;
	for(i=0;i<MAX_CFG_ITEMS;i++)
	{
		if( (config[i].grp == grp) && (config[i].id == id) ) config[i].v.u32 = v.u32;
		if( config[i].grp == 255 ) return;
	}
}


u08 cfg_get_index_by_grp_and_id(uint8 grp, uint8 id)
{
	u08 i;
	for(i=0;i<MAX_CFG_ITEMS;i++)
	{
		if( (config[i].grp == grp) && (config[i].id == id) ) break;;
		if( config[i].grp == 255 ) {i=MAX_CFG_ITEMS; break;}
	}
	usb_printf("cfg index of %d,%d = %d\n",grp,id,i);
	return i;
}

u08 cfg_get_u08_by_index(uint8 index)
{
	return config[index].v.u08;
}

u16 cfg_get_u16_by_index(uint8 index)
{
	return config[index].v.u16;
}

u32 cfg_get_u32_by_index(uint8 index)
{
	return config[index].v.u32;
}

s08 cfg_get_s08_by_index(uint8 index)
{
	return config[index].v.s08;
}

s16 cfg_get_s16_by_index(uint8 index)
{
	return config[index].v.s16;
}


flt cfg_get_flt_by_index(uint8 index)
{
	return config[index].v.f;
}


void cfg_get_value_by_index(u08 index, u08 type_id, void* value)
{
	switch(type_id)
	{
		case u08_VALUE:
			*((u08*)value) = config[index].v.u08;
			break;
		case s08_VALUE:
			*((s08*)value) = config[index].v.s08;
			break;
		case u16_VALUE:
			*((u16*)value) = config[index].v.u16;
			break;
		case s16_VALUE:
			*((s16*)value) = config[index].v.s16;
			break;
		case u32_VALUE:
			*((u32*)value) = config[index].v.u32;
			break;
		case s32_VALUE:
			*((s32*)value) = config[index].v.s32;
			break;
		case flt_VALUE:
			*((flt*)value) = config[index].v.f;
			break;
	}
}

u08 cfg_get_u08_by_grp_id(uint8 grp, uint8 id)
{
	u08 i;
	for(i=0;i<=MAX_CFG_ITEMS;i++)
	{
		if( (config[i].grp == grp) && (config[i].id == id) ) return config[i].v.u08;
		if(config[i].grp == 255) return 0; //reached the end
	}
	return 0;
}

s16 cfg_get_s16_by_grp_id(uint8 grp, uint8 id)
{
	u08 i;
	for(i=0;i<=MAX_CFG_ITEMS;i++)
	{
		if( (config[i].grp == grp) && (config[i].id == id) ) return config[i].v.s16;
		if(config[i].grp == 255) return 0; //reached the end
	}
	return 0;
}



#ifdef WIN32
//#pragma warning( pop ) 
#endif
