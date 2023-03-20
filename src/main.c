/*
 * psp2wpp
 * Copyright (C) 2022, Princess of Sleeping
 */

#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/threadmgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/io/stat.h>
#include <psp2/paf.h>
#include <psp2/sysmodule.h>
#include <psp2/vshbridge.h>
#include <taihen.h>
#include "wave.h"


#define HookImport(module_name, library_nid, func_nid, func_name) taiHookFunctionImport(&func_name ## _ref, module_name, library_nid, func_nid, func_name ## _patch)


int wave_config_read(const char *path);
extern SceWaveParam wave_param;

SceBool is_enso;
SceUID thid = -1, sema_id = -1;

SceFColor *select_wave_color;

void *(* ScePafGraphics_4E038C05)(void);
void *(* FUN_8109defe)(void *a1, int a2);
int (* FUN_810a8080)(void *a1, const char *name, const SceWaveParam *pWaveParam);


int resolve_offsets(void){

	int res;
	tai_module_info_t moduleInfo;
	SceKernelModuleInfo info;
	void *text, *data;

	moduleInfo.size = sizeof(moduleInfo);

	res = taiGetModuleInfo("SceShell", &moduleInfo);
	if(res < 0){
		return res;
	}

	sceKernelGetModuleInfo(moduleInfo.modid, &info);

	data = info.segments[1].vaddr;

	switch(moduleInfo.module_nid){
	case 0xB45216F4: // Tool 3.72-I
		select_wave_color = (SceFColor *)(data + (0x815442B0 - 0x81532000)); // 0x122B0
		break;
	default:
		sceClibPrintf("%s fingerprint 0x%08X\n", "SceShell", moduleInfo.module_nid);
		select_wave_color = NULL;
		break;
	}

	moduleInfo.size = sizeof(moduleInfo);

	res = taiGetModuleInfo("ScePaf", &moduleInfo);
	if(res < 0){
		return res;
	}

	sceKernelGetModuleInfo(moduleInfo.modid, &info);

	text = info.segments[0].vaddr;

	switch(moduleInfo.module_nid){
	case 0xCD679177: // CEX 3.60
	case 0x0305714C: // CEX 3.61
	case 0xE15A86ED: // CEX 3.63
	case 0x73F90499: // CEX 3.65
	case 0xE7A89CA9: // CEX 3.67
	case 0x66EC0C5D: // CEX 3.68
	case 0x52A49770: // CEX 3.69
	case 0x78DF8738: // CEX 3.70
	case 0x09111E96: // CEX 3.71
	case 0xD5A425AF: // CEX 3.72
	case 0x9CE470B4: // CEX 3.73
	case 0x0DBFBA9B: // CEX 3.74
		ScePafGraphics_4E038C05 = text + (0x7f7b8 | 1);
		FUN_8109defe = text + (0x9d40e | 1);
		FUN_810a8080 = text + (0xa7590 | 1);
		break;
	case 0xD4018D85: // Tool 3.60-I tool_for_cex
	case 0xA081BFE1: // Tool 3.72-I
		ScePafGraphics_4E038C05 = text + (0x802A8 | 1);
		FUN_8109defe = text + (0x9defe | 1);
		FUN_810a8080 = text + (0xa8080 | 1);
		break;
	default:
		sceClibPrintf("%s fingerprint 0x%08X\n", "ScePaf", moduleInfo.module_nid);
		ScePafGraphics_4E038C05 = NULL;
		FUN_8109defe = NULL;
		FUN_810a8080 = NULL;
		break;
	}

	return 0;
}

const char * const wave_name_list[0x20] = {
	"effects_default_0512_01",
	"iboot",
	"effects_0406_ruby",
	"effects_0407_purple_01",
	"effects_0408_01_orange",
	"effects_0411_purple_02",
	"effects_0415_A5",
	"effects_0518_dalia",
	"effects_0518_gray_FLAT",
	"effects_0518_ruby",
	"effects_A1",
	"effects_black",
	"effects_blue_FLAT",
	"effects_blueTB_0513_01",
	"effects_bluewhite_05",
	"effects_casis",

	"effects_green_0512_01",
	"effects_koiao_0512_01",
	"effects_maintosh",
	"effects_marrygold",
	"effects_peach_0512_001",
	"effects_purple_01",
	"effects_purplered",
	"effects_yellow_0512_01",
	"effects_redTB_0513_01",
	"effects_limegreen_0513_02",
	"effects_turquoise_0513_01",
	"effects_color_vari_2g_01",
	"effects_color_vari_2g_02",
	"effects_color_vari_2g_03",
	"effects_color_vari_2g_04",
	"effects_original_01"
};

const SceUInt32 wave_index_translation_list[0x20] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08,
	0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
	0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19,
	0x1A, 0x17, 0x07, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F
};

int load_waveparam(void){

	int res;
	char path[0x100];

	if(ScePafGraphics_4E038C05 == NULL){
		return -1;
	}

	for(int i=0;i<0x1F;i++){

		sceClibMemset(&wave_param, 0, sizeof(wave_param));

		res = -1;

		if(res < 0 && (vshSblAimgrIsTool() == SCE_TRUE || vshSblAimgrIsTest() == SCE_TRUE)){
			sceClibSnprintf(path, sizeof(path), "%sdata/waveparams/%d.txt", "host0:", i);
			res = wave_config_read(path);
		}

		if(res < 0){
			sceClibSnprintf(path, sizeof(path), "%sdata/waveparams/%d.txt", "ux0:", i);
			res = wave_config_read(path);
		}

		if(res >= 0){
			SceUInt32 wave_index = wave_index_translation_list[i];

			if(select_wave_color != NULL && i < 0x1F){
				select_wave_color[i].r = wave_param.selecter[0].r;
				select_wave_color[i].g = wave_param.selecter[0].g;
				select_wave_color[i].b = wave_param.selecter[0].b;
				select_wave_color[i].a = wave_param.selecter[0].a;
				select_wave_color[0x1F + i].r = wave_param.selecter[1].r;
				select_wave_color[0x1F + i].g = wave_param.selecter[1].g;
				select_wave_color[0x1F + i].b = wave_param.selecter[1].b;
				select_wave_color[0x1F + i].a = wave_param.selecter[1].a;
			}

			wave_param.magic       = SCE_WAVE_PARAM_MAGIC;
			wave_param.version     = 1;
			wave_param.color_index = wave_index;
			wave_param.unk_0x0C    = 0;

			void *wave_ctx = ScePafGraphics_4E038C05();
			void *wave_info = FUN_8109defe(*(void **)(wave_ctx), 0);

			FUN_810a8080(*(void **)(wave_info), wave_name_list[wave_index], &wave_param);
		}
	}

	return 0;
}

int waveparam_update_thread(SceSize argc, ScePVoid argp){

	int res;

	while(1){
		res = sceKernelWaitSema(sema_id, 1, NULL);
		if(res < 0){
			sceClibPrintf("sceKernelWaitSema 0x%X\n", res);
			continue;
		}

		load_waveparam();

		if(is_enso == SCE_TRUE){

			sceClibMemset(&wave_param, 0, sizeof(wave_param));

			do {
				res = wave_config_read("ux0:data/waveparam.txt");
				if(res == 0 || res != 0x80010002){
					break;
				}

				res = wave_config_read("sd0:data/waveparam.txt");
				if(res == 0 || res != 0x80010002){
					break;
				}

				res = wave_config_read("host0:data/waveparam.txt");
				if(res == 0 || res != 0x80010002){
					break;
				}
			} while(0);

			if(res < 0){
				// sceClibPrintf("waveparam.txt not found\n");
			}else{
				wave_param.magic       = SCE_WAVE_PARAM_MAGIC;
				wave_param.version     = 1;
				wave_param.color_index = 0x1F;
				wave_param.unk_0x0C    = 0;

				void *wave_ctx = ScePafGraphics_4E038C05();
				void *wave_info = FUN_8109defe(*(void **)(wave_ctx), 0);

				FUN_810a8080(*(void **)(wave_info), wave_name_list[0x1F], &wave_param);
			}
		}

		SceUInt32 prev_index = scePafGraphicsCurrentWave;
		scePafGraphicsCurrentWave = 0x20;
		scePafGraphicsUpdateCurrentWave(prev_index, 1.0f);
	}

	return 0;
}

int psp2wpp_spawn_reload_server(void){

	int res;

	res = sceKernelCreateSema("psp2wpp_sema", SCE_KERNEL_ATTR_OPENABLE, 0, 1, NULL);
	if(res < 0){
		return res;
	}

	sema_id = res;

	do {
		res = sceKernelCreateThread("waveparam_update_thread", waveparam_update_thread, 0x78, 0x2000, 0, 0, NULL);
		if(res < 0){
			break;
		}

		thid = res;

		res = sceKernelStartThread(thid, 0, NULL);
		if(res >= 0){
			return 0;
		}

		sceKernelDelayThread(thid);
		thid = -1;
	} while(0);

	sceKernelDelayThread(sema_id);
	sema_id = -1;

	return res;
}

tai_hook_ref_t ScePafGraphics_45A01FA1_ref;
int ScePafGraphics_45A01FA1_patch(SceWaveParam *pParam){

	int res;

	sceClibMemset(&wave_param, 0, sizeof(wave_param));

	do {
		res = wave_config_read("ux0:data/waveparam.txt");
		if(res == 0 || res != 0x80010002){
			break;
		}

		res = wave_config_read("sd0:data/waveparam.txt");
		if(res == 0 || res != 0x80010002){
			break;
		}

		res = wave_config_read("host0:data/waveparam.txt");
		if(res == 0 || res != 0x80010002){
			break;
		}
	} while(0);

	if(res == 0){
		wave_param.magic       = SCE_WAVE_PARAM_MAGIC;
		wave_param.version     = 1;
		wave_param.color_index = 0x1F;
		wave_param.unk_0x0C    = 0;

		pParam = &wave_param;

		// custom waveparam dump point.
	}

	return TAI_CONTINUE(int, ScePafGraphics_45A01FA1_ref, pParam);
}

tai_hook_ref_t ScePafMisc_B9FB9BD6_ref;
int ScePafMisc_B9FB9BD6_patch(void **dst, const char *path, int a3, int a4, void *a5){

	int res;
	SceIoStat stat;

	if(0 == sce_paf_strcmp(path, "pd0:wave/waveparam.bin")){

		load_waveparam();

		res = sceIoGetstat(path, &stat);

		if(res < 0){
			path = "ux0:data/waveparam.bin";
			res = sceIoGetstat(path, &stat);
		}

		if(res < 0){
			path = "sd0:data/waveparam.bin";
			res = sceIoGetstat(path, &stat);
		}

		if(res < 0 && (vshSblAimgrIsTool() != 0 || vshSblAimgrIsTest() != 0)){
			path = "host0:data/waveparam.bin";
			res = sceIoGetstat(path, &stat);
		}
	}

	return TAI_CONTINUE(int, ScePafMisc_B9FB9BD6_ref, dst, path, a3, a4, a5);
}

tai_hook_ref_t sceSysmoduleLoadModuleInternalWithArg_ref;
int sceSysmoduleLoadModuleInternalWithArg_patch(SceSysmoduleInternalModuleId id, SceSize args, void *argp, void *unk){

	int res = TAI_CONTINUE(int, sceSysmoduleLoadModuleInternalWithArg_ref, id, args, argp, unk);

	if(res >= 0 && id == SCE_SYSMODULE_INTERNAL_PAF){
		HookImport("SceShell", 0xa070d6a7, 0x45A01FA1, ScePafGraphics_45A01FA1);
		HookImport("SceShell", 0x3d643ce8, 0xB9FB9BD6, ScePafMisc_B9FB9BD6);

		resolve_offsets();
		psp2wpp_spawn_reload_server();
	}

	return res;
}

tai_hook_ref_t vshIdStorageLookup_ref;
int vshIdStorageLookup_patch(SceUInt32 leaf, SceUInt32 offset, ScePVoid data, SceSize length){

	int res = TAI_CONTINUE(int, vshIdStorageLookup_ref, leaf, offset, data, length);

	if((res >= 0 || res == 0x80230005) && leaf == 0x116 && offset == 0 && data != NULL && length == 4){

		if(res < 0){
			*(SceUInt32 *)(data) = 0;
			res = 0;
		}

		if(0){ // Have pre-theme color
			*(SceUInt8 *)(data) |= 1;
			*(SceUInt8 *)(data + 1) = 0x0; // color index (0~0x1F)
		}

		if(1){ // Have pd0:wave/waveparam.bin
			*(SceUInt8 *)(data) |= 2;
		}

		if(0){ // Have pre-installed theme (pd0:theme)
			*(SceUInt8 *)(data) |= 4;
		}
	}

	return res;
}

int psp2wpp_main(void){

	int res;
	SceUInt64 syscall[2];

	if(_vshKernelSearchModuleByName("SceSysStateMgr", syscall) >= 0){
		is_enso = SCE_TRUE;
	}else{
		is_enso = SCE_FALSE;
	}

	if(is_enso == SCE_TRUE){
		HookImport("SceShell", 0x03FCF19D, 0xC3C26339, sceSysmoduleLoadModuleInternalWithArg);
		HookImport("SceShell", 0x35C5ACD4, 0x58BA5A8D, vshIdStorageLookup);
	}else{
		res = resolve_offsets();
		if(res < 0){
			return res;
		}

		res = psp2wpp_spawn_reload_server();
		if(res < 0){
			return res;
		}

		load_waveparam();

		SceUInt32 prev_index = scePafGraphicsCurrentWave;
		scePafGraphicsCurrentWave = 0x20;
		scePafGraphicsUpdateCurrentWave(prev_index, 1.0f);
	}

	return 0;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp){

	if(psp2wpp_main() < 0){
		return SCE_KERNEL_START_FAILED;
	}

	return SCE_KERNEL_START_SUCCESS;
}
