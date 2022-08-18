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


int wave_config_read(void);
extern SceWaveParam wave_param;


tai_hook_ref_t ScePafGraphics_45A01FA1_ref;
int ScePafGraphics_45A01FA1_patch(SceWaveParam *pParam){

	int res;

	res = wave_config_read();
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

SceUID thid = -1, sema_id = -1;

int color_change_thread(SceSize argc, ScePVoid argp){

	int res;

	while(1){
		res = sceKernelWaitSema(sema_id, 1, NULL);
		if(res < 0){
			continue;
		}

		res = wave_config_read();
		if(res < 0){
			continue;
		}

		wave_param.magic       = SCE_WAVE_PARAM_MAGIC;
		wave_param.version     = 1;
		wave_param.color_index = 0x1F;
		wave_param.unk_0x0C    = 0;

		TAI_CONTINUE(int, ScePafGraphics_45A01FA1_ref, &wave_param);
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
		res = sceKernelCreateThread("color_change_thread", color_change_thread, 0x60, 0x4000, 0, 0, NULL);
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

int psp2wpp_main(void){

	int res;

	res = psp2wpp_spawn_reload_server();
	if(res < 0){
		return res;
	}

	HookImport("SceShell", 0x03FCF19D, 0xC3C26339, sceSysmoduleLoadModuleInternalWithArg);
	HookImport("SceShell", 0x35C5ACD4, 0x58BA5A8D, vshIdStorageLookup);

	return 0;
}

void _start() __attribute__ ((weak, alias("module_start")));
int module_start(SceSize args, void *argp){

	if(psp2wpp_main() < 0){
		return SCE_KERNEL_START_FAILED;
	}

	return SCE_KERNEL_START_SUCCESS;
}
