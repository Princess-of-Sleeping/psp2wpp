/*
 * psp2wpp
 * Copyright (C) 2022, Princess of Sleeping
 */

#ifndef _PSP2_WAVE_PARAM_H_
#define _PSP2_WAVE_PARAM_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <psp2/types.h>


#define SCE_WAVE_PARAM_MAGIC (0x61776976)

typedef struct SceWaveParam { // size is 0x2A0-bytes
	SceUInt32 magic;          // "viwa" on string
	SceUInt32 version;        // 1 only
	SceUInt32 color_index;    // 0x1F only
	SceUInt32 unk_0x0C;       // 0 only
	SceFColor selecter[2];

	// offset: 0x30
	struct {
		SceFColor color;
		SceFloat32 diffuse_coeff;
		SceFloat32 specular_coeff;
		SceFloat32 specular_power;
		SceFloat32 fresnel_front;
		SceFloat32 fresnel_control;
		SceFloat32 fresnel_coeff;
		SceFloat32 refraction_coeff;
		SceFloat32 refraction_amount;
		SceFloat32 second_reflection;
	} Material;

	SceUInt32 padding_0x64[3]; // unused

	// offset: 0x70
	struct {
		SceFloat32 theta_phi[2];
		SceUInt32 padding_0x78[2]; // unused
		SceFPlane attn;
		SceFColor ambient_color;
		SceFColor diffuse_color;
		SceFloat32 distance;
		SceFloat32 fade;
	} PointLightSphere;

	SceUInt32 padding_0xB8[2]; // unused

	// offset: 0xC0
	/*
	 * 0: Fog at the bottom of the screen
	 * 1: Fog at the up of the screen?
	 */
	struct {
		SceFColor color;
		SceFPlane density;
		SceFColor light_color; // The color of the fog that covers the center of the screen
		SceFloat32 d_scale;
		SceFloat32 d_offset;
		SceUInt32 padding[2]; // unused
	} Fog[2];
	struct {
		SceFloat32 theta_phi[2];
		SceUInt32 padding[2]; // unused
		SceFColor sun_color; // The color of the light that hits(?) the wave
		SceFColor zenith_color;
		SceFColor horizon_color;
		SceFloat32 distance;
		SceFloat32 sun_power;
		SceFloat32 sun_control;
		SceFloat32 wave_fog_alpha;
		SceFloat32 radius;
		SceFloat32 xscale;
		SceFloat32 side_angle;
		SceFloat32 horizon_angle;
		SceFloat32 horizon_blend_range;
		SceFloat32 horizon_curvature;
		SceFloat32 sky_blend_start;
		SceFloat32 sky_blend_range;
		SceFloat32 pos_distortion_scale;
		SceFloat32 grad_distortion_scale;
		SceFloat32 horizon_distortion_scale;
	} Sky;
	SceUInt32 padding_0x1BC; // unused
	struct {
		SceFloat32 gravity;
		SceFloat32 A;
		SceFloat32 logA;
		SceFloat32 patch_size; // Wave moving coeff
		SceFloat32 wind_speed;
		SceFloat32 wind_dir;
		SceFloat32 time_step;
	} FFTWave;
	SceUInt32 padding_0x1DC; // unused
	struct {
		SceFQuaternion translation;
		SceFQuaternion rotation;
		SceFQuaternion scale;
		SceFColor center_color; // wave color
		SceFColor edge_color;
		SceFQuaternion cross_section;
		SceFQuaternion cross_section_flat;
		SceFloat32 distortion_scale;
		SceFloat32 global_scale;
		SceFloat32 uv_scale;
		SceFloat32 uv_rotate;
		SceFloat32 center_power;
		SceFloat32 edge_power;
		SceFloat32 decay;
		SceFloat32 shadow_nor_blend;
	} WaveInstance;
	struct {
		SceFloat32 edge_fog[2];
		SceUInt32 padding[2]; // unused
		SceFloat32 pitch_max;
		SceFloat32 pitch_rest;
		SceFloat32 pitch_min;
		SceFloat32 pitch_debug;
		SceFloat32 roll_max;
		SceFloat32 roll_max_zone;
		SceFloat32 zoom;
		SceFloat32 camera_chase;
	} WaveRenderer;
} SceWaveParam;


#ifdef __cplusplus
}
#endif

#endif /* _PSP2_WAVE_PARAM_H_ */
