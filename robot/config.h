#ifndef _config_h_
#define _config_h_


#ifdef __cplusplus 
extern "C" {
#endif
	
#include "typedefs.h"

#define MAX_CFG_ITEMS 90

#define DEFINE_CFG(type,name,grp,id) static type name = 0; static u08 name##_cfg_idx = 0; const u08 name##_grp = grp; const u08 name##_id= id
#define PREPARE_CFG(type,name) name##_cfg_idx = cfg_get_index_by_grp_and_id(name##_grp,name##_id)
#define UPDATE_CFG(type,name) name = cfg_get_##type##_by_index(name##_cfg_idx)

#define DEFINE_CFG2(type,name,grp,id) \
	static type name = 0; \
	static u08 name##_cfg_idx = 0; \
	const u08 name##_t = type##_VALUE; \
	const u08 name##_grp = grp; \
	const u08 name##_id= id

#define PREPARE_CFG2(name) \
	name##_cfg_idx = cfg_get_index_by_grp_and_id(name##_grp,name##_id)

#define UPDATE_CFG2(name) \
	cfg_get_value_by_index(name##_cfg_idx, name##_t, (void*)(&name))

#ifdef WIN32
//#define __attribute__ (a)
#endif


#define U08_VALUE 1
#define S08_VALUE 2
#define U16_VALUE 3
#define S16_VALUE 4
#define U32_VALUE 5
#define S32_VALUE 6
#define FLT_VALUE 7
	
#define u08_VALUE 1
#define s08_VALUE 2
#define u16_VALUE 3
#define s16_VALUE 4
#define u32_VALUE 5
#define s32_VALUE 6
#define flt_VALUE 7


#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef union
{
	uint32 u32;
	sint32 s32;
	uint16 u16;
	sint16 s16;
	uint8  u08;
	sint8  s08;
	float    f;
}
#ifndef WIN32
__attribute__((__packed__))
#endif
t_config_value;
#ifdef WIN32
#pragma pack(pop)
#endif

	
#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 grp;
	uint8 id;
	uint8  type;
	char name[31];
	union
	{
		uint32 u32;
		sint32 s32;
		uint16 u16;
		sint16 s16;
		uint8  u08;
		sint8  s08;
		float    f;
	} v;
} 
#ifndef WIN32 
__attribute__((__packed__)) 
#endif
t_config_flash;
#ifdef WIN32
#pragma pack(pop)
#endif


//workaround for WIN32
#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 grp;
	uint8 id;
	uint8  type;
	char name[31];
	float    f;
} 
#ifndef WIN32 
__attribute__((__packed__)) 
#endif
t_config_flash_float_only;
#ifdef WIN32
#pragma pack(pop)
#endif



	
#ifdef WIN32
#pragma pack(push)
#pragma pack(1)
#endif
typedef struct
{
	uint8 grp;
	uint8 id;
	union
	{
		uint32 u32;
		sint32 s32;
		uint16 u16;
		sint16 s16;
		uint8  u08;
		sint8  s08;
		float    f;
	} v;
}
#ifndef WIN32
__attribute__((__packed__))
#endif
t_config;
#ifdef WIN32
#pragma pack(pop)
#endif





#ifdef WIN32
extern t_config_flash config[MAX_CFG_ITEMS];
extern t_config_flash config_flash[MAX_CFG_ITEMS];
extern t_config_flash_float_only config_float_only[MAX_CFG_ITEMS];
#else
extern t_config config[MAX_CFG_ITEMS];
extern const t_config_flash config_flash[MAX_CFG_ITEMS];
#endif


extern  u08 cfg_get_u08_by_index(uint8 index);
extern  u16 cfg_get_u16_by_index(uint8 index);
extern  u32 cfg_get_u32_by_index(uint8 index);
extern  s08 cfg_get_s08_by_index(uint8 index);
extern  s16 cfg_get_s16_by_index(uint8 index);
extern  flt cfg_get_flt_by_index(uint8 index);

extern  void cfg_get_value_by_index(u08 index, u08 type_id, void* value);

extern void cfg_set_s16_by_index(uint8 index, s16 value);
extern void cfg_set_value_by_grp_id(uint8 grp, uint8 id, t_config_value v);

extern u08 cfg_get_u08_by_grp_id(uint8 grp, uint8 id);
extern u16 cfg_get_u16_by_grp_id(uint8 grp, uint8 id);
extern u32 cfg_get_u32_by_grp_id(uint8 grp, uint8 id);
extern s08 cfg_get_s08_by_grp_id(uint8 grp, uint8 id);
extern s16 cfg_get_s16_by_grp_id(uint8 grp, uint8 id);

extern u08 cfg_get_index_by_grp_and_id(uint8 grp, uint8 id);


extern void cfg_test(void);
extern void cfg_init(void);


#define CFG_S16_LM_SPEED_TURN_LEFT_CORNER cfg_get_s16_by_id(100)
#define CFG_S16_RM_SPEED_TURN_LEFT_CORNER cfg_get_s16_by_id(101)


#ifdef __cplusplus 
}
#endif


#endif