
#ifndef _DICT_RPC_
#define _DICT_RPC_

typedef int (*dict_rpc_send_fn)(void *, char *, size_t);

int dict_rpc_vmsg(dict_rpc_send_fn, void *,
                  const char *, const char *, const char *,
                  const char *, va_list);
int dict_rpc_msg(dict_rpc_send_fn, void *,
                 const char *, const char *, const char *,
                 const char *, ...);
int dict_rpc_verror(dict_rpc_send_fn, void *,
                   const char *, const char *,
                   int, const char *, va_list);
int dict_rpc_error(dict_rpc_send_fn, void *,
                   const char *, const char *,
                   int, const char *, ...);
int dict_rpc_vok(dict_rpc_send_fn, void *,
                 const char *, const char *,
                 const char *, va_list);
int dict_rpc_ok(dict_rpc_send_fn, void *,
                const char *, const char *,
                const char *, ...);
int dict_rpc_status(dict_rpc_send_fn, void *,
                    const char *, ...);
int dict_rpc_request(dict_rpc_send_fn, void *, const char *,
                     void (*)(void *, dict), void *, const char *, ...);

enum dict_rpc_arg_type {
    DICT_RPC_ARG_TYPE_STRING,
    DICT_RPC_ARG_TYPE_INTEGER,
    DICT_RPC_ARG_TYPE_BOOLEAN,
    DICT_RPC_ARG_TYPE_ARRAY,
};

struct dict_rpc_arg_desc {
    const char *name;
    enum dict_rpc_arg_type type;
    int optional;
    union {
        char *string;
        char *integer;
        char *boolean;
    } defval;
};

#define DICT_RPC_ARG_DEFVAL_STRING(s) { .string = (s) }
#define DICT_RPC_ARG_DEFVAL_INTEGER(n) { .integer = (#n) }
#define DICT_RPC_ARG_DEFVAL_BOOLEAN(b) { .boolean = (#b) }

typedef int (*dict_rpc_command_fn)(void *, const char *, const char *,
                                   dict, void *);

struct dict_rpc_command {
    const char *command;
    dict_rpc_command_fn fn;
    void *opaque;
    struct dict_rpc_arg_desc *args;
    unsigned long long int flags;
};

typedef int (*dict_rpc_execute_fn)(dict_rpc_send_fn, void *,
                                   struct dict_rpc_command *, const char *,
                                   dict, void *);

void dict_rpc_init(void);
void dict_rpc_cb_exit(void);
int dict_rpc_process_input(dict_rpc_execute_fn, dict_rpc_send_fn, void *, dict,
                           struct dict_rpc_command *, size_t, void *);
int dict_rpc_process_input_buffer(dict_rpc_execute_fn, dict_rpc_send_fn,
                                  void *, const char *,
                                  struct dict_rpc_command *, size_t, void *);
#define DICT_EXECUTE_ERROR_SUPPRESS 0x80000000

#endif  /* _DICT_RPC_ */
