#ifndef PRINT_H
#define PRINT_H

/*
 *  Example:
 *  file_print(stdout, arg_str_len(buf, count));
 *
 *  this function has two parts first part is the setter part
 *  which set the print context so the other functions know where
 *  are they writing second part is the writer part in this macro
 *  it starts with '_print_ctx_string(ctx_pointer, "" ' and this
 *  is left open so in expr which is the part where parsing/formating
 *  done for this example its arg_str_len which expands to
 *  "") || _print_ctx_string_len(...) || _print_ctx_string(...
 *  which left open again so when those to merge "" and "" concatinated
 *  so its a empty string write which costs us only a function call and
 *  if check we need || chaing on bools via this we can continue or stop
 *  the pipeline if || receives stop(1 or non-zero value) this will end
 *  the execution if it receives continue(0) this will continue and
 *  every function sets the error code in the ctx variable and after
 *  pipeline end or get a stop signal it moves to end which gives the user
 *  the current status this is an enum which has 3 option right now
 *  PRINT_SUCCESS = 0 and PRINT_PARTIAL which returns if one of the writing
 *  can be a memcpy or fwrite or the string opartion is only done half way
 *  the last one is PRINT_UNKNOWN_FORMAT which should never happen this
 *  happens if the ctx gets a type which is different that file, buffer
 *  or a string
 */

#include <stddef.h> /* For size_t etc */
#include <stdio.h> /* For file writes */

#ifdef PRINT_USE_CAPITAL_HEX
#define _PRINT_HEX_TABLE "0123456789ABCDEF"
#else
#define _PRINT_HEX_TABLE "0123456789abcdef"
#endif /* PRINT_USE_CAPTIAL_HEX */

#ifndef PRINT_FLOAT_SEPARATOR
#define PRINT_FLOAT_SEPARATOR "."
#endif

#ifndef PRINT_NULL_STRING
#define PRINT_NULL_STRING "(null)"
#endif

/* Separated for things like \r\n etc */
#ifndef PRINT_NEW_LINE
#define PRINT_NEW_LINE "\n"
#endif

/* Note: #define PRINT_USE_PRINTF_FOR_FP
 *        macro only sets the printf backend for floating pointer numbers
 */

/* #define PRINT_USE_PRINTF
 * This macro sets backend to printf("format", var) style and dont uses the
 * custom backend instead it uses libc's printf function
 */
#ifdef PRINT_USE_PRINTF
#ifndef PRINT_USE_PRINTF_FOR_FP
#define PRINT_USE_PRINTF_FOR_FP
#endif /* PRINT_USE_PRINTF_FOR_FP */

#ifndef PRINT_HEX_FORMAT
#ifdef PRINT_USE_CAPITAL_HEX
#define PRINT_HEX_FORMAT "%llX"
#else
#define PRINT_HEX_FORMAT "%llx"
#endif
#endif /* PRINT_HEX_FORMAT */

#ifndef PRINT_OCT_FORMAT
#define PRINT_OCT_FORMAT "%llo"
#endif /* PRINT_OCT_FORMAT */

#ifndef PRINT_FP_FORMAT
#define PRINT_FP_FORMAT "%lf"
#endif /* PRINT_FP_FORMAT */

#ifndef PRINT_DEC_FORMAT
#define PRINT_DEC_FORMAT "%lld"
#endif /* PRINT_DEC_FORMAT */
#endif /* PRINT_USE_PRINTF */

/* Via this macro the library uses string or basic
 * implementations of that functions or use specified functions
 */
#ifdef PRINT_DONT_USE_STRING_H
extern void *_print_memcpy(void *dst, const void *src, size_t n);
extern size_t _print_strlen(const char *str);
#ifndef PRINT_MEMCPY
#define PRINT_MEMCPY _print_memcpy
#endif /* PRINT_MEMCPY */

#ifndef PRINT_STRLEN
#define PRINT_STRLEN _print_strlen
#endif /* PRINT_STRLEN */

#else /* if PRINT_DONT_USE_STRING_H is not declared use string.h */

#include <string.h>
#ifndef PRINT_MEMCPY
#define PRINT_MEMCPY memcpy
#endif /* PRINT_MEMCPY */

#ifndef PRINT_STRLEN
#define PRINT_STRLEN strlen
#endif /* PRINT_STRLEN */

#endif /* PRINT_DONT_USE_STRING_H */

#ifndef PRINT_TMP_SIZE
/* Note: This limit will be enough for now integers representation
 *       uses maxium ~25 and fp can go up to ~40
 */
/* Note: Internal buffers dont rely on null termination and they use size instead */
#define PRINT_TMP_SIZE 64
#endif /* PRINT_TMP_SIZE */

/* Expected signature is (str_pointer, write_buf, len)
 * str_pointer is a pointer to string which we store as void *str
 * write_buf is a pointer to buffer which will be written stored expected to be void *buf
 * len is a amount of items (in bytes)
 * retun value: expected a size_t return which is how many bytes are written
 */
#define PRINT_DUMMY_STRING_OUT(a, b, c) (0)
/* If we dont have a string grow or string append function dont do anything */
#ifndef PRINT_STRING_OUT
#error "PRINT_STRING_OUT is not defiened if you are not using please set it to dummy!"
#endif /* PRINT_STRING_OUT */

/* Needed for signed numbers with the maximum avaible architecture length */
#if defined(_WIN64)
typedef __int64 _print_max_ssize_t;
#elif defined(_MSC_VER)
typedef __int64 _print_max_ssize_t;
#else
typedef long _print_max_ssize_t;
#endif

/* Success should always be zero otherwise '||' chain wont work */
enum print_error_e {
    PRINT_SUCCESS = 0,
    PRINT_PARTIAL,
    PRINT_UNKNOWN_FORMAT
};

enum _print_ctx_type_e {
    _PRINT_CTX_FILE,
    _PRINT_CTX_BUF,
    _PRINT_CTX_STR
};

typedef enum print_error_e print_error_t;
typedef enum _print_ctx_type_e _print_ctx_type_t;
typedef struct _print_ctx_s _print_ctx_t;

struct _print_ctx_s {
    _print_ctx_type_t type;
    union {
        FILE* file;
        struct {
            void *ptr;
            size_t count;
        } buf;
        /* Use users' string pointer we dont know the type here */
        void*  str;
    } target;
    /* We use this variable because we need to know
     * how much to jump in the buffer/ptr for printing functions
     */
    size_t written;
    print_error_t status;
};

enum _print_state_e {
    _print_state_continue = 0,
    _print_state_stop = 1
};

typedef enum _print_state_e _print_state_t;

/* _print_ctx_* functions uses bool if there is an error they return true(1) which in that case
 * ctx.status is set to error and via || other functions are skipped
 */

/* Publicly usable ctx formaters */
extern _print_state_t _print_ctx_string_len
(_print_ctx_t *ctx, const char *str, size_t len);
extern _print_state_t _print_ctx_string
(_print_ctx_t *ctx, const char *str);
extern _print_state_t _print_ctx_fp
(_print_ctx_t *ctx, double num);
extern _print_state_t _print_ctx_dec
(_print_ctx_t *ctx, _print_max_ssize_t neg_num);
extern _print_state_t _print_ctx_oct
(_print_ctx_t *ctx, size_t num);
extern _print_state_t _print_ctx_hex
(_print_ctx_t *ctx, size_t num);

/* User should just use use_print_ctx if they want
 * they can chage the name or use the type but
 * that might broke somethings
 */
#ifndef _PRINT_CTX_NAME
#define _PRINT_CTX_NAME print_h_print_ctx
#endif /* _PRINT_CTX_NAME */
#define use_print_ctx _print_ctx_t _PRINT_CTX_NAME = {0}

/* Context constructors */
#define __file_print_set(file_ptr, expr) \
    _PRINT_CTX_NAME.type = _PRINT_CTX_FILE, _PRINT_CTX_NAME.target.file = (file_ptr), \
    _PRINT_CTX_NAME.status = PRINT_SUCCESS, _PRINT_CTX_NAME.written = 0, \
    _print_ctx_string(&_PRINT_CTX_NAME, "" expr "")

#define __buf_print_set(buf_ptr, buf_count, expr) \
    _PRINT_CTX_NAME.type = _PRINT_CTX_BUF, _PRINT_CTX_NAME.target.buf.ptr = (buf_ptr), \
    _PRINT_CTX_NAME.target.buf.count = (buf_count), \
    _PRINT_CTX_NAME.status = PRINT_SUCCESS, _PRINT_CTX_NAME.written = 0, \
    _print_ctx_string(&_PRINT_CTX_NAME, "" expr "")

#define __str_print_set(string, expr) \
    _PRINT_CTX_NAME.type = _PRINT_CTX_STR, _PRINT_CTX_NAME.target.str = (string), \
    _PRINT_CTX_NAME.status = PRINT_SUCCESS, _PRINT_CTX_NAME.written = 0, \
    _print_ctx_string(&_PRINT_CTX_NAME, "" expr "")

/* Public interface for print functions */
/* usage of *_print(out, some_variable) wont work user
 * needs to specify the type via arg_* functions
 */
#define file_print(file, expr) \
    (__file_print_set(file, expr), _PRINT_CTX_NAME.status)

#define file_print_wrote(file, wrote, expr) \
    (__file_print_set(file, expr), (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

#define buf_print(buf_ptr, buf_count, expr) \
    (__buf_print_set(buf_ptr, buf_count, expr), _PRINT_CTX_NAME.status)

#define buf_print_wrote(buf_ptr, buf_count, wrote, expr) \
    (__buf_print_set(buf_ptr, buf_count, expr), \
    (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

#define str_print(string, expr) \
    (__str_print_set(string, expr), _PRINT_CTX_NAME.status)

#define str_print_wrote(string, wrote, expr) \
    (__str_print_set(string, expr), (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

/* Same print function but with automatic new line */

#define _print_append_newline() _print_ctx_string_len(&_PRINT_CTX_NAME, PRINT_NEW_LINE, 1)
#define file_println(file, expr) \
    (__file_print_set(file, expr) || _print_append_newline(), _PRINT_CTX_NAME.status)

#define file_println_wrote(file, wrote, expr) \
    (__file_print_set(file, expr) || _print_append_newline(),\
    (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

#define buf_println(buf_ptr, buf_count, expr) \
    (__buf_print_set(buf_ptr, buf_count, expr) || _print_append_newline(),\
    _PRINT_CTX_NAME.status)

#define buf_println_wrote(buf_ptr, buf_count, wrote, expr) \
    (__buf_print_set(buf_ptr, buf_count, expr) || _print_append_newline(), \
    (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

#define str_println(string, expr) \
    (__str_print_set(string, expr) || _print_append_newline(), _PRINT_CTX_NAME.status)

#define str_println_wrote(string, wrote, expr) \
    (__str_print_set(string, expr) || _print_append_newline(), \
    (wrote) = _PRINT_CTX_NAME.written, _PRINT_CTX_NAME.status)

/* Formater setters - User can make their own formats by passing function and variable names */
#define _print_format_1(func, var) \
   "") || func(&_PRINT_CTX_NAME, var) || _print_ctx_string(&_PRINT_CTX_NAME, ""

#define _print_format_2(func, var, var2) \
   "") || func(&_PRINT_CTX_NAME, var, var2) || _print_ctx_string(&_PRINT_CTX_NAME, ""

/* Public formats */
#define arg_fp(num) _print_format_1(_print_ctx_fp, num)
#define arg_hex(num) _print_format_1(_print_ctx_hex, num)
#define arg_oct(num) _print_format_1(_print_ctx_oct, num)
#define arg_dec(num) _print_format_1(_print_ctx_dec, num)
#define arg_str(str) _print_format_1(_print_ctx_string, str)
#define arg_str_len(str, len) _print_format_2(_print_ctx_string_len, str, len)
#define arg_str_lit(str) _print_format_2(_print_ctx_string_len, str, sizeof(str) - 1)

#endif /* PRINT_H */

#ifdef PRINT_IMPLEMENTATION

/* Note: Will be undef at the end */
#define _print_call_printf(count, buf, format, num) \
do { \
    size_t n = snprintf(buf, sizeof(buf), format, num); \
    if (count < 0) { return _print_state_stop; } \
    count = (size_t)n; \
    if (count >= sizeof(buf)) { count = sizeof(buf)-1; } \
} while(0);

/* Internal helpers */
static char* _print_format_base
(char* out_end, size_t num, size_t base);
static char* _print_format_double
(char *end, double num);

#ifdef PRINT_DONT_USE_STRING_H
/* Implement needed internals */

void *_print_memcpy
(void *dst, const void *src, size_t n)
{
    char *d = dst;
    const char *s = src;

    while(n--) *d++ = *s++;
    return dst;
}

size_t _print_strlen
(const char *str)
{
    size_t n = 0;
    while(*str++) n++;
    return n;
}

#endif /* PRINT_USE_STRING_H */

_print_state_t _print_ctx_string_len
(_print_ctx_t *ctx, const char *str, size_t len)
{
    size_t wrote = 0, max = 0;

    switch (ctx->type) {
        case _PRINT_CTX_FILE: {
            wrote = fwrite(str, sizeof(char), len, ctx->target.file);
        } break;
        case _PRINT_CTX_STR: {
            wrote = PRINT_STRING_OUT(ctx->target.str, str, len);
        } break;
        case _PRINT_CTX_BUF: {
            max = (ctx->written < ctx->target.buf.count)
                ? ctx->target.buf.count - ctx->written : 0;
            max = (max > len) ? len : max;
            wrote = max;
            PRINT_MEMCPY((char *)ctx->target.buf.ptr + ctx->written, str, max);
        } break;
        default: { ctx->status = PRINT_UNKNOWN_FORMAT; return _print_state_stop; }
    }

    ctx->written += wrote;
    ctx->status = (wrote == len) ? PRINT_SUCCESS : PRINT_PARTIAL;
    return (ctx->status) ? _print_state_stop : _print_state_continue;
}

_print_state_t _print_ctx_hex
(_print_ctx_t *ctx, size_t num)
{
    char buf[PRINT_TMP_SIZE], *out = buf; size_t count = 0;

#ifdef PRINT_USE_PRINTF
    _print_call_printf(count, buf, PRINT_HEX_FORMAT, num);
#else
    out = _print_format_base(buf + PRINT_TMP_SIZE, num, 16);
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return _print_ctx_string_len(ctx, out, count);
}

_print_state_t _print_ctx_oct
(_print_ctx_t *ctx, size_t num)
{
    char buf[PRINT_TMP_SIZE], *out = buf; size_t count = 0;

#ifdef PRINT_USE_PRINTF
    _print_call_printf(count, buf, PRINT_OCT_FORMAT, num);
#else
    out = _print_format_base(buf + PRINT_TMP_SIZE, num, 8);
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return _print_ctx_string_len(ctx, out, count);
}

_print_state_t _print_ctx_dec
(_print_ctx_t *ctx, _print_max_ssize_t neg_num)
{
    char buf[PRINT_TMP_SIZE], *out = buf;
    size_t count = 0;

#ifdef PRINT_USE_PRINTF
    _print_call_printf(count, buf, PRINT_DEC_FORMAT, neg_num);
#else
    size_t num = 0;
    char is_negative = 0;

    /* Check for negative number */
    if(neg_num < 0) {
        is_negative = 1;
        num = (size_t)(-(neg_num + 1)) + 1;
    } else { num = (size_t)neg_num; }

    out = _print_format_base(buf + PRINT_TMP_SIZE, num, 10);

    if(is_negative) { *--out = '-'; }
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return _print_ctx_string_len(ctx, out, count);
}

_print_state_t _print_ctx_fp
(_print_ctx_t *ctx, double num)
{
    char buf[PRINT_TMP_SIZE], *out = buf;
    char is_negative = 0;
    size_t count = 0;

#ifdef PRINT_USE_PRINTF_FOR_FP
    _print_call_printf(count, buf, PRINT_FP_FORMAT, num);
#else
    /* Check for negative number */
    if(num < 0) { is_negative = 1; num *= -1; }
    out = _print_format_double(buf + PRINT_TMP_SIZE, num);
    if(is_negative) { *--out = '-'; }
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return _print_ctx_string_len(ctx, out, count);
}

_print_state_t _print_ctx_string
(_print_ctx_t *ctx, const char *str)
{
    /* Skip if empty string like "" which saves little bit of time */
    /* This happens a lot because of conjunctions */
    if(str && !*str) { return _print_state_continue; }
    if(str) {
        return _print_ctx_string_len(ctx, str, PRINT_STRLEN(str));
    }
    return _print_ctx_string_len(ctx, PRINT_NULL_STRING, sizeof(PRINT_NULL_STRING) - 1);
}

/* TODO: This will be slow because we can not do masking for hex and oct
 *       so we need different function for them
 */
/* Note: num can not be negative and '-' should added afterwards by the caller */
/* Note: We expect caller to give enough buffer to us and pointer should be at
 *       the end of the buffer a normal integer representation wont pass ~25 characters
 *       and we are using 64(default one) so it should be good.
 */
static char* _print_format_base
(char *end, size_t num, size_t base)
{
    static const char hex[] = _PRINT_HEX_TABLE;
    do {
        *--end = hex[num % base];
        num /= base;
    } while(num);
    return end;
}

static char* _print_format_double(char *end, double num)
{
    size_t integer;
    size_t fraction;
    int i;
    const char *sep = PRINT_FLOAT_SEPARATOR;
    sep += sizeof(PRINT_FLOAT_SEPARATOR) - 1;

    integer = (size_t)num;
    num -= (double)integer;
    fraction = (size_t)(num * 1000000.0 + 0.5);

    /* Handle rounding overflow */
    if (fraction >= 1000000) {
        integer++;
        fraction -= 1000000;
    }

    for(i = 0; i < 6; i++) {
        *--end = '0' + (fraction % 10);
        fraction /= 10;
    }

    while((*--end = *--sep));
    /* Take back the null byte */
    end++;

    return _print_format_base(end, integer, 10);
}

#undef _print_call_printf
#endif /* PRINT_IMPLEMENTATION */
