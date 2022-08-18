/*
 * psp2wpp
 * Copyright (C) 2022, Princess of Sleeping
 */

#include <psp2/kernel/clib.h>
#include <psp2/paf.h>
#include <psp2/vshbridge.h>
#include "wave.h"
#include "config_parser.h"


SceWaveParam wave_param;

typedef struct WaveMemberStrMap {
	const char *name;
	float *value;
} WaveMemberStrMap;

typedef struct WavePartMap {
	const char *name;
	const WaveMemberStrMap *map;
	int n;
} WavePartMap;

#define wave_map_entry(p, m) { \
		.name = #m, \
		.value = &((p).m) \
	}

#define wave_map_entry2(p, m, name) { \
		.name = #name, \
		.value = &((p).m) \
	}

const WaveMemberStrMap waveMemberStrMap_top[] = {
	wave_map_entry(wave_param, selecter[0].r),
	wave_map_entry(wave_param, selecter[0].g),
	wave_map_entry(wave_param, selecter[0].b),
	wave_map_entry(wave_param, selecter[0].a),
	wave_map_entry(wave_param, selecter[0].r),
	wave_map_entry(wave_param, selecter[1].g),
	wave_map_entry(wave_param, selecter[1].b),
	wave_map_entry(wave_param, selecter[1].a)
};

const WaveMemberStrMap waveMemberStrMap_material[] = {
	wave_map_entry(wave_param.Material, color.r),
	wave_map_entry(wave_param.Material, color.g),
	wave_map_entry(wave_param.Material, color.b),
	wave_map_entry(wave_param.Material, color.a),
	wave_map_entry(wave_param.Material, diffuse_coeff),
	wave_map_entry(wave_param.Material, specular_coeff),
	wave_map_entry(wave_param.Material, specular_power),
	wave_map_entry(wave_param.Material, fresnel_front),
	wave_map_entry(wave_param.Material, fresnel_control),
	wave_map_entry(wave_param.Material, fresnel_coeff),
	wave_map_entry(wave_param.Material, refraction_coeff),
	wave_map_entry(wave_param.Material, refraction_amount),
	wave_map_entry(wave_param.Material, second_reflection)
};

const WaveMemberStrMap waveMemberStrMap_pointLightSphere[] = {
	wave_map_entry(wave_param.PointLightSphere, theta_phi[0]),
	wave_map_entry(wave_param.PointLightSphere, theta_phi[1]),
	wave_map_entry(wave_param.PointLightSphere, distance),
	wave_map_entry(wave_param.PointLightSphere, attn.a),
	wave_map_entry(wave_param.PointLightSphere, attn.b),
	wave_map_entry(wave_param.PointLightSphere, attn.c),
	wave_map_entry(wave_param.PointLightSphere, attn.d),
	wave_map_entry(wave_param.PointLightSphere, ambient_color.r),
	wave_map_entry(wave_param.PointLightSphere, ambient_color.g),
	wave_map_entry(wave_param.PointLightSphere, ambient_color.b),
	wave_map_entry(wave_param.PointLightSphere, ambient_color.a),
	wave_map_entry(wave_param.PointLightSphere, diffuse_color.r),
	wave_map_entry(wave_param.PointLightSphere, diffuse_color.g),
	wave_map_entry(wave_param.PointLightSphere, diffuse_color.b),
	wave_map_entry(wave_param.PointLightSphere, diffuse_color.a),
	wave_map_entry(wave_param.PointLightSphere, fade)
};


const WaveMemberStrMap waveMemberStrMap_fog1[] = {
	wave_map_entry(wave_param.Fog[1], color.r),
	wave_map_entry(wave_param.Fog[1], color.g),
	wave_map_entry(wave_param.Fog[1], color.b),
	wave_map_entry(wave_param.Fog[1], color.a),
	wave_map_entry(wave_param.Fog[1], density.a),
	wave_map_entry(wave_param.Fog[1], density.b),
	wave_map_entry(wave_param.Fog[1], density.c),
	wave_map_entry(wave_param.Fog[1], density.d),
	wave_map_entry(wave_param.Fog[1], light_color.r),
	wave_map_entry(wave_param.Fog[1], light_color.g),
	wave_map_entry(wave_param.Fog[1], light_color.b),
	wave_map_entry(wave_param.Fog[1], light_color.a),
	wave_map_entry(wave_param.Fog[1], d_scale),
	wave_map_entry(wave_param.Fog[1], d_offset)
};

const WaveMemberStrMap waveMemberStrMap_fog0[] = {
	wave_map_entry(wave_param.Fog[0], color.r),
	wave_map_entry(wave_param.Fog[0], color.g),
	wave_map_entry(wave_param.Fog[0], color.b),
	wave_map_entry(wave_param.Fog[0], color.a),
	wave_map_entry(wave_param.Fog[0], density.a),
	wave_map_entry(wave_param.Fog[0], density.b),
	wave_map_entry(wave_param.Fog[0], density.c),
	wave_map_entry(wave_param.Fog[0], density.d),
	wave_map_entry(wave_param.Fog[0], light_color.r),
	wave_map_entry(wave_param.Fog[0], light_color.g),
	wave_map_entry(wave_param.Fog[0], light_color.b),
	wave_map_entry(wave_param.Fog[0], light_color.a),
	wave_map_entry(wave_param.Fog[0], d_scale),
	wave_map_entry(wave_param.Fog[0], d_offset)
};

const WaveMemberStrMap waveMemberStrMap_Sky[] = {
	wave_map_entry(wave_param.Sky, theta_phi[0]),
	wave_map_entry(wave_param.Sky, theta_phi[1]),
	wave_map_entry(wave_param.Sky, distance),
	wave_map_entry(wave_param.Sky, sun_color.r),
	wave_map_entry(wave_param.Sky, sun_color.g),
	wave_map_entry(wave_param.Sky, sun_color.b),
	wave_map_entry(wave_param.Sky, sun_color.a),
	wave_map_entry(wave_param.Sky, sun_power),
	wave_map_entry(wave_param.Sky, sun_control),
	wave_map_entry(wave_param.Sky, zenith_color.r),
	wave_map_entry(wave_param.Sky, zenith_color.g),
	wave_map_entry(wave_param.Sky, zenith_color.b),
	wave_map_entry(wave_param.Sky, zenith_color.a),
	wave_map_entry(wave_param.Sky, horizon_color.r),
	wave_map_entry(wave_param.Sky, horizon_color.g),
	wave_map_entry(wave_param.Sky, horizon_color.b),
	wave_map_entry(wave_param.Sky, horizon_color.a),
	wave_map_entry(wave_param.Sky, wave_fog_alpha),
	wave_map_entry(wave_param.Sky, radius),
	wave_map_entry(wave_param.Sky, xscale),
	wave_map_entry(wave_param.Sky, side_angle),
	wave_map_entry(wave_param.Sky, horizon_angle),
	wave_map_entry(wave_param.Sky, horizon_blend_range),
	wave_map_entry(wave_param.Sky, horizon_curvature),
	wave_map_entry(wave_param.Sky, sky_blend_start),
	wave_map_entry(wave_param.Sky, sky_blend_range),
	wave_map_entry(wave_param.Sky, pos_distortion_scale),
	wave_map_entry(wave_param.Sky, grad_distortion_scale),
	wave_map_entry(wave_param.Sky, horizon_distortion_scale)
};

const WaveMemberStrMap waveMemberStrMap_FFTWave[] = {
	wave_map_entry(wave_param.FFTWave, gravity),
	wave_map_entry(wave_param.FFTWave, A),
	wave_map_entry(wave_param.FFTWave, logA),
	wave_map_entry(wave_param.FFTWave, patch_size),
	wave_map_entry(wave_param.FFTWave, wind_speed),
	wave_map_entry(wave_param.FFTWave, wind_dir),
	wave_map_entry(wave_param.FFTWave, time_step)
};

const WaveMemberStrMap waveMemberStrMap_WaveInstance[] = {
	wave_map_entry(wave_param.WaveInstance, distortion_scale),
	wave_map_entry(wave_param.WaveInstance, translation.x),
	wave_map_entry(wave_param.WaveInstance, translation.y),
	wave_map_entry(wave_param.WaveInstance, translation.z),
	wave_map_entry(wave_param.WaveInstance, translation.w),
	wave_map_entry(wave_param.WaveInstance, rotation.x),
	wave_map_entry(wave_param.WaveInstance, rotation.y),
	wave_map_entry(wave_param.WaveInstance, rotation.z),
	wave_map_entry(wave_param.WaveInstance, rotation.w),
	wave_map_entry(wave_param.WaveInstance, scale.x),
	wave_map_entry(wave_param.WaveInstance, scale.y),
	wave_map_entry(wave_param.WaveInstance, scale.z),
	wave_map_entry(wave_param.WaveInstance, scale.w),
	wave_map_entry(wave_param.WaveInstance, global_scale),
	wave_map_entry(wave_param.WaveInstance, uv_scale),
	wave_map_entry(wave_param.WaveInstance, uv_rotate),
	wave_map_entry(wave_param.WaveInstance, center_color.r),
	wave_map_entry(wave_param.WaveInstance, center_color.g),
	wave_map_entry(wave_param.WaveInstance, center_color.b),
	wave_map_entry(wave_param.WaveInstance, center_color.a),
	wave_map_entry(wave_param.WaveInstance, center_power),
	wave_map_entry(wave_param.WaveInstance, edge_color.r),
	wave_map_entry(wave_param.WaveInstance, edge_color.g),
	wave_map_entry(wave_param.WaveInstance, edge_color.b),
	wave_map_entry(wave_param.WaveInstance, edge_color.a),
	wave_map_entry(wave_param.WaveInstance, edge_power),
	wave_map_entry(wave_param.WaveInstance, cross_section.x),
	wave_map_entry(wave_param.WaveInstance, cross_section.y),
	wave_map_entry(wave_param.WaveInstance, cross_section.z),
	wave_map_entry(wave_param.WaveInstance, cross_section.w),
	wave_map_entry(wave_param.WaveInstance, cross_section_flat.x),
	wave_map_entry(wave_param.WaveInstance, cross_section_flat.y),
	wave_map_entry(wave_param.WaveInstance, cross_section_flat.z),
	wave_map_entry(wave_param.WaveInstance, cross_section_flat.w),
	wave_map_entry(wave_param.WaveInstance, decay),
	wave_map_entry(wave_param.WaveInstance, shadow_nor_blend)
};

const WaveMemberStrMap waveMemberStrMap_WaveRenderer[] = {
	wave_map_entry(wave_param.WaveRenderer, edge_fog[0]),
	wave_map_entry(wave_param.WaveRenderer, edge_fog[1]),
	wave_map_entry(wave_param.WaveRenderer, pitch_max),
	wave_map_entry(wave_param.WaveRenderer, pitch_rest),
	wave_map_entry(wave_param.WaveRenderer, pitch_min),
	wave_map_entry(wave_param.WaveRenderer, pitch_debug),
	wave_map_entry(wave_param.WaveRenderer, roll_max),
	wave_map_entry(wave_param.WaveRenderer, roll_max_zone),
	wave_map_entry(wave_param.WaveRenderer, zoom),
	wave_map_entry(wave_param.WaveRenderer, camera_chase)
};

const WavePartMap wavePartMap[] = {
	{
		.name = "Select",
		.map = waveMemberStrMap_top,
		.n = sizeof(waveMemberStrMap_top) / sizeof(waveMemberStrMap_top[0])
	},
	{
		.name = "Material",
		.map = waveMemberStrMap_material,
		.n = sizeof(waveMemberStrMap_material) / sizeof(waveMemberStrMap_material[0])
	},
	{
		.name = "PointLightSphere",
		.map = waveMemberStrMap_pointLightSphere,
		.n = sizeof(waveMemberStrMap_pointLightSphere) / sizeof(waveMemberStrMap_pointLightSphere[0])
	},
	{
		.name = "Fog1",
		.map = waveMemberStrMap_fog1,
		.n = sizeof(waveMemberStrMap_fog1) / sizeof(waveMemberStrMap_fog1[0])
	},
	{
		.name = "Fog0",
		.map = waveMemberStrMap_fog0,
		.n = sizeof(waveMemberStrMap_fog0) / sizeof(waveMemberStrMap_fog0[0])
	},
	{
		.name = "Sky",
		.map = waveMemberStrMap_Sky,
		.n = sizeof(waveMemberStrMap_Sky) / sizeof(waveMemberStrMap_Sky[0])
	},
	{
		.name = "FFTWave",
		.map = waveMemberStrMap_FFTWave,
		.n = sizeof(waveMemberStrMap_FFTWave) / sizeof(waveMemberStrMap_FFTWave[0])
	},
	{
		.name = "WaveInstance",
		.map = waveMemberStrMap_WaveInstance,
		.n = sizeof(waveMemberStrMap_WaveInstance) / sizeof(waveMemberStrMap_WaveInstance[0])
	},
	{
		.name = "WaveRenderer",
		.map = waveMemberStrMap_WaveRenderer,
		.n = sizeof(waveMemberStrMap_WaveRenderer) / sizeof(waveMemberStrMap_WaveRenderer[0])
	}
};



const WavePartMap *wavePartMap_curr = NULL;

int conf_reader_type(const char *config, ConfReaderCtx **conf_reader_ctx, const ConfReaderFunc *pInfo){

	int cmd_len = sce_paf_strlen(pInfo->cmd_name);

	if(sce_paf_strncmp(config, pInfo->cmd_name, cmd_len) != 0)
		return -1;

	config = &config[cmd_len];

	while(*config == ' ' || *config == '\t'){
		config++;
	}

	const char *config_path_s = config;

	while(*config != '\r' && *config != '\n' && *config != 0){
		config++;
	}

	int arg_len = config - config_path_s;

	if(arg_len == 0 && arg_len >= 0x400){
		return -1;
	}

	char path[0x400];

	path[arg_len] = 0;
	sce_paf_strncpy(path, config_path_s, arg_len);

	// sceClibPrintf("%s\n", path);

	for(int i=0;i<(sizeof(wavePartMap)/sizeof(wavePartMap[0]));i++){
		if(sce_paf_strcmp(path, wavePartMap[i].name) == 0){
			wavePartMap_curr = &(wavePartMap[i]);
			return 0;
		}
	}

	return -1;
}

int conf_reader_set_value(const char *config, ConfReaderCtx **conf_reader_ctx, const ConfReaderFunc *pInfo){

	if(wavePartMap_curr == NULL){
		return -1;
	}

	int cmd_len = sce_paf_strlen(pInfo->cmd_name);

	if(sce_paf_strncmp(config, pInfo->cmd_name, cmd_len) != 0)
		return -1;

	config = &config[cmd_len];

	while(*config == ' ' || *config == '\t'){
		config++;
	}

	const char *config_path_s = config;

	while(*config != ' ' && *config != '\t' && *config != '\r' && *config != '\n' && *config != 0){
		config++;
	}

	if((config - config_path_s) == 0){
		return -1;
	}

	char path[0x400];
	sce_paf_memset(path, 0, sizeof(path));
	sce_paf_strncpy(path, config_path_s, config - config_path_s);

	config_path_s = config;

	while(*config != '\r' && *config != '\n' && *config != 0){
		config++;
	}

	if((config - config_path_s) == 0){
		return -1;
	}

	char str_value[0x400];
	sce_paf_memset(str_value, 0, sizeof(str_value));
	sce_paf_strncpy(str_value, config_path_s, config - config_path_s);

	double value = sce_paf_strtod(str_value, NULL);

	// sceClibPrintf("%s: %s = %f\n", pInfo->cmd_name, path, ((float)value));

	for(int i=0;i<wavePartMap_curr->n;i++){
		if(sce_paf_strcmp(path, wavePartMap_curr->map[i].name) == 0){
			*(wavePartMap_curr->map[i].value) = (float)value;
			break;
		}
	}

	return 0;
}

const ConfReaderFunc conf_read_funcs[] = {
	{
		.cmd_name = "type",
		.cb = conf_reader_type,
		.argp = NULL,
		.argi = 0
	},
	{
		.cmd_name = "set_value",
		.cb = conf_reader_set_value,
		.argp = NULL,
		.argi = 0
	}
};

int wave_config_read(void){

	int res;

	res = config_parse_by_path("ux0:data/waveparam.txt", conf_read_funcs, sizeof(conf_read_funcs) / sizeof(conf_read_funcs[0]));
	if(res != 0x80010002){
		return res;
	}

	res = config_parse_by_path("sd0:data/waveparam.txt", conf_read_funcs, sizeof(conf_read_funcs) / sizeof(conf_read_funcs[0]));
	if(res != 0x80010002){
		return res;
	}

	if(vshSblAimgrIsTool() != 0 || vshSblAimgrIsTest() != 0){
		res = config_parse_by_path("host0:data/waveparam.txt", conf_read_funcs, sizeof(conf_read_funcs) / sizeof(conf_read_funcs[0]));
		if(res != 0x80010002){
			return res;
		}
	}

	if(res == 0x80010002){
		return 1;
	}

	return 0;
}
