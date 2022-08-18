/*
 * PS Vita config script reader
 * Copyright (C) 2020, Princess of Sleeping
 */

#include <psp2/types.h>
#include <psp2/kernel/clib.h>
#include <psp2/io/fcntl.h>
#include <psp2/paf.h>
#include "config_parser.h"


int config_parse(const char *pConfig, SceSize size, const ConfReaderFunc *pConfReaderFunc, unsigned int num){

	if(pConfig == NULL || (size - 1) < 0){
		sceClibPrintf("[%-15s] Invalid config\n", "Error");
		return -1;
	}

	ConfReaderCtx *conf_reader_ctx = (ConfReaderCtx *)sce_paf_malloc(sizeof(ConfReaderCtx));
	sce_paf_memset(conf_reader_ctx, 0, sizeof(ConfReaderCtx));

	conf_reader_ctx->prev = (ConfReaderCtx *)sce_paf_malloc(sizeof(ConfReaderCtx));
	sce_paf_memset(conf_reader_ctx->prev, 0, sizeof(ConfReaderCtx));

	ConfReaderCtx **ppConfReaderCtx = &conf_reader_ctx;

	const char *config_read = pConfig;

	int is_in_comment = 0, error_out = 0, ignore_error = 0;

	// uint32_t val = 0;

	int dbg_line = 1;

	while(config_read < (char *)((uintptr_t)pConfig + size) && error_out == 0 && (*ppConfReaderCtx)->is_read_end == 0){

		if(*config_read == '#'){
			config_read++;
			is_in_comment = 1;

		}else if(sce_paf_strncmp(config_read, "-", 1) == 0 && is_in_comment == 0){

			char ch = config_read[1];

			if(ch != ' '){
				error_out = 1;
			}else{
				config_read++;

				while(*config_read == ' ' || *config_read == '\t')
					config_read++;

				if((*ppConfReaderCtx)->is_do_nothing == 0)
					ignore_error = 1;
			}

		}else if(*config_read == '\r' || *config_read == '\n'){

			dbg_line++;

			if(config_read[0] == '\r' && config_read[1] == '\n')
				dbg_line--;

			config_read++;
			is_in_comment = 0;
			ignore_error = 0;
		}else{
			if(is_in_comment == 0){
				error_out = 1;
				for(int i=0;i<num;i++){

					char loc_val_name[0x80];

					sce_paf_snprintf(loc_val_name, sizeof(loc_val_name) - 1, "%s", pConfReaderFunc[i].cmd_name);

					if(sce_paf_strncmp(config_read, loc_val_name, sce_paf_strlen(loc_val_name)) != 0)
						continue;

					char ch = config_read[sce_paf_strlen(loc_val_name)];

					if(ch != ' ' && ch != '\t' && ch != '\r' && ch != '\n')
						continue;

					error_out = 0;

					if(pConfReaderFunc[i].cb(config_read, ppConfReaderCtx, &pConfReaderFunc[i]) < 0 && ignore_error == 0)
						error_out = 2;

				/*
					if(ignore_error != 0)
						sceClibPrintf("^^ ignore error ^^\n");
				*/
					is_in_comment = 1;
					break;
				}
			}
			config_read++;
		}
	}

	conf_reader_ctx = *ppConfReaderCtx;

	while(conf_reader_ctx != NULL){
		void *pCtxPrev = conf_reader_ctx->prev;
		sce_paf_free(conf_reader_ctx);
		conf_reader_ctx = pCtxPrev;
	}

	if(error_out != 0){
		if(error_out == 1)
			sceClibPrintf("[%-15s] The config contains invalid syntax\n", "Error");

		sceClibPrintf("[%-15s] Line:%d\n", "Error", dbg_line);
		return -1;
	}

	return 0;
}

int config_parse_by_path(const char *path, const ConfReaderFunc *pConfReaderFunc, unsigned int num){

	int res, fd, readbyte;
	char *config = NULL;
	SceOff offset;

	fd = sceIoOpen(path, SCE_O_RDONLY, 0);
	if(fd < 0){
		// sceClibPrintf("config open failed\n");
		return 0x80010002;
	}

	readbyte = 0;

	do {
		offset = sceIoLseek(fd, 0LL, SCE_SEEK_END);
		if(offset < 0){
			sceClibPrintf("sceIoLseek: 0x%llX\n", offset);
			break;
		}

		SceSize config_size = (SceSize)offset;
		if(config_size != 0){
			config = (char *)sce_paf_malloc(config_size + 0x3F);
			sce_paf_memset(config, 0, config_size + 0x3F);
		}

		offset = sceIoLseek(fd, 0LL, SCE_SEEK_SET);
		if(offset < 0){
			break;
		}

		readbyte = sceIoRead(fd, config, config_size);
	} while(0);

	sceIoClose(fd);

	res = config_parse(config, readbyte, pConfReaderFunc, num);

	sce_paf_free(config);

	return res;
}
