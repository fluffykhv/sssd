#include "sbus/sssd_dbus.h"
#include "responder/common/responder_cmd.h"
#include "util/btreemap.h"

/* SSS_DOMAIN_DELIM can be specified in config.h */
#include "config.h"
#ifndef SSS_DOMAIN_DELIM
#define SSS_DOMAIN_DELIM '@'
#endif


int sss_process_init(TALLOC_CTX *mem_ctx,
                     struct event_context *ev,
                     struct confdb_ctx *cdb,
                     struct sbus_method sss_sbus_methods[],
                     struct sss_cmd_table sss_cmds[],
                     const char *sss_pipe_name,
                     const char *confdb_socket_path,
                     struct sbus_method dp_methods[]);

int sss_parse_name(TALLOC_CTX *memctx,
                   const char *fullname,
                   struct btreemap *domain_map,
                   const char **domain, const char **name);
