
#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ConfReaderCtx {
	struct ConfReaderCtx *prev;
	int is_do_nothing;
	int is_read_end;
	char if_char[0x24]; // if not used, can remove
	int flags;
} ConfReaderCtx;

typedef struct ConfReaderFunc ConfReaderFunc;

typedef int (* ConfReaderCallback)(const char *config, ConfReaderCtx **conf_reader_ctx, const ConfReaderFunc *pInfo);

typedef struct ConfReaderFunc {
	const char *cmd_name;
	ConfReaderCallback cb;
	void *argp;
	int   argi;
} ConfReaderFunc;

int config_parse(const char *pConfig, SceSize size, const ConfReaderFunc *pConfReaderFunc, unsigned int num);
int config_parse_by_path(const char *path, const ConfReaderFunc *pConfReaderFunc, unsigned int num);

#ifdef __cplusplus
}
#endif

#endif /* _CONFIG_PARSER_H_ */
