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

/* PRINTH__* printh__ used for internals should not touched or changed
 * PRINT_* print_* are externals and safe to use
 */


/* User can specify custom types as maxium types to both unsigned and signed
 * integers but not to floating point value we only use double there
 */
/* Note: If user uses a C version which is different than C89 it uses intmax_t
 * types to be fixed sized otherwise it uses size_t for unsigned and extension
 * for signed integers
 */
#ifdef PRINT_USE_CUSTOM_MAX_UNSIGNED_TYPE
typedef PRINT_USE_CUSTOM_MAX_UNSIGNED_TYPE printh__umax_t;
#else /* PRINT_USE_CUSTOM_MAX_UNSIGNED_TYPE */
#ifndef __STDC_VERSION__
#include <stddef.h> /* for size_t */
typedef size_t printh__umax_t;
#else /* __STDC_VERSION__ */
#include <stdint.h> /* for uintmax_t */
typedef uintmax_t printh__umax_t;
#endif /* __STDC_VERSION__ */
#endif /* PRINT_USE_CUSTOM_MAX_UNSIGNED_TYPE */

#ifdef PRINT_USE_CUSTOM_MAX_SIGNED_TYPE
typedef PRINT_USE_CUSTOM_MAX_SIGNED_TYPE printh__max_t;
#else
#ifndef __STDC_VERSION__
#if defined(_WIN64)
typedef __int64 printh__max_t;
#elif defined(_MSC_VER)
typedef __int64 printh__max_t;
#else /* defined(_WIN64) */
typedef long printh__max_t;
#endif /* defined(_WIN64) */
#else  /* __STDC_VERSION__ */
#include <stdint.h> /* for intmax_t */
typedef intmax_t printh__max_t;
#endif /* __STDC_VERSION__ */
#endif /* PRINT_USE_CUSTOM_MAX_SIGNED_TYPE */

#ifdef PRINT_USE_CAPITAL_HEX
#define PRINTH__HEX_TABLE "0123456789ABCDEF"
#else
#define PRINTH__HEX_TABLE "0123456789abcdef"
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

/* printf and printf format macros */

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

/* end of printf and printf format macros */

/* <string.h> header macros */

/* Via this macro the library uses string or basic
 * implementations of that functions or use specified functions
 */
#ifdef PRINT_DONT_USE_STRING_H
extern void *printh__memcpy(void *dst, const void *src, printh__umax_t n);
extern printh__umax_t printh__strlen(const char *str);
#ifndef PRINT_MEMCPY
#define PRINT_MEMCPY printh__memcpy
#endif /* PRINT_MEMCPY */

#ifndef PRINT_STRLEN
#define PRINT_STRLEN printh__strlen
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

/* end of <string.h> macros */

#ifdef PRINT_DONT_USE_STDIO_H

/* If nobody sets it */
#ifndef PRINT_FILE_TYPE
#define PRINT_FILE_TYPE void
#endif /* PRINT_FILE_TYPE */

#else /* PRINT_DONT_USE_STDIO_H */
#include <stdio.h>
/* Its not mandatory to use standards if user wants to something else they can set it */

/* Note: is uses a pointer not the actual type itself so forward declaration and opaque types allowed */
#ifndef PRINT_FILE_TYPE
#define PRINT_FILE_TYPE FILE
#endif /* PRINT_FILE_TYPE */

/* fwrite stlye signature is expected */
#ifndef PRINT_FILE_OUT
#define PRINT_FILE_OUT fwrite
#endif /* PRINT_FILE_OUT */

#endif /* PRINT_DONT_USE_STDIO_H */

#define PRINT_DUMMY_FILE_OUT(buffer_pointer, size_of_char, amount_of_char, file_pointer) (0)

#ifndef PRINT_FILE_OUT
#error "PRINT_FILE_OUT is not defiened if you are not using please set it to dummy!"
#endif /* PRINT_FILE_OUT */

/* <stdio.h> header macros */

/* end of <stdio.h> macros */

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
 * retun value: expected a printh__umax_t or equivalent return which is how many bytes are written
 */
#define PRINT_DUMMY_STRING_OUT(string_pointer, buffer_pointer, amount_of_bytes) (0)
/* If we dont have a string grow or string append function dont do anything */
#ifndef PRINT_STRING_OUT
#error "PRINT_STRING_OUT is not defiened if you are not using please set it to dummy!"
#endif /* PRINT_STRING_OUT */

/* Success should always be zero otherwise '||' chain wont work */
enum print_error_e {
    print_success = 0,
    print_partial,
    print_unknown_format
};

enum printh__ctx_type_e {
    PRINTH__CTX_FILE,
    PRINTH__CTX_BUF,
    PRINTH__CTX_STR
};

typedef enum print_error_e print_error_t;
typedef enum printh__ctx_type_e printh__ctx_type_t;
typedef struct printh__ctx_s printh__ctx_t;

struct printh__ctx_s {
    printh__ctx_type_t type;
    union {
        PRINT_FILE_TYPE *file;
        struct {
            void *ptr;
            printh__umax_t count;
        } buf;
        /* Use users' string pointer we dont know the type here */
        void*  str;
    } target;
    /* We use this variable because we need to know
     * how much to jump in the buffer/ptr for printing functions
     */
    printh__umax_t written;
    print_error_t status;
};

enum printh__state_e {
    printh__state_continue = 0,
    printh__state_stop = 1
};

typedef enum printh__state_e printh__state_t;

/* _print_ctx_* functions uses bool if there is an error they return true(1) which in that case
 * ctx.status is set to error and via || other functions are skipped
 */

/* Publicly usable ctx formaters */
extern printh__state_t printh__ctx_string_len
(printh__ctx_t *ctx, const char *str, printh__umax_t len);
extern printh__state_t printh__ctx_string
(printh__ctx_t *ctx, const char *str);
extern printh__state_t printh__ctx_fp
(printh__ctx_t *ctx, double num);
extern printh__state_t printh__ctx_dec
(printh__ctx_t *ctx, printh__max_t neg_num);
extern printh__state_t printh__ctx_oct
(printh__ctx_t *ctx, printh__umax_t num);
extern printh__state_t printh__ctx_hex
(printh__ctx_t *ctx, printh__umax_t num);

/* User should just use use_print_ctx if they want
 * they can chage the name or use the type but
 * that might broke somethings
 */
#ifndef PRINTH__CTX_NAME
#define PRINTH__CTX_NAME print_h_print_ctx
#endif /* PRINTH__CTX_NAME */
#define use_print_ctx printh__ctx_t PRINTH__CTX_NAME = {0}

/* Context constructors */
#define printh__file_print_set(file_ptr, expr) \
    PRINTH__CTX_NAME.type = PRINTH__CTX_FILE, PRINTH__CTX_NAME.target.file = (file_ptr), \
    PRINTH__CTX_NAME.status = print_success, PRINTH__CTX_NAME.written = 0, \
    printh__ctx_string(&PRINTH__CTX_NAME, "" expr "")

#define printh__buf_print_set(buf_ptr, buf_count, expr) \
    PRINTH__CTX_NAME.type = PRINTH__CTX_BUF, PRINTH__CTX_NAME.target.buf.ptr = (buf_ptr), \
    PRINTH__CTX_NAME.target.buf.count = (buf_count), \
    PRINTH__CTX_NAME.status = print_success, PRINTH__CTX_NAME.written = 0, \
    printh__ctx_string(&PRINTH__CTX_NAME, "" expr "")

#define printh__str_print_set(string, expr) \
    PRINTH__CTX_NAME.type = PRINTH__CTX_STR, PRINTH__CTX_NAME.target.str = (string), \
    PRINTH__CTX_NAME.status = print_success, PRINTH__CTX_NAME.written = 0, \
    printh__ctx_string(&PRINTH__CTX_NAME, "" expr "")

/* Public interface for print functions */
/* usage of *_print(out, some_variable) wont work user
 * needs to specify the type via arg_* functions
 */
#define file_print(file, expr) \
    (printh__file_print_set(file, expr), PRINTH__CTX_NAME.status)

#define file_print_wrote(file, wrote, expr) \
    (printh__file_print_set(file, expr), (wrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

#define buf_print(buf_ptr, buf_count, expr) \
    (printh__buf_print_set(buf_ptr, buf_count, expr), PRINTH__CTX_NAME.status)

#define buf_print_wrote(buf_ptr, buf_count, wrote, expr) \
    (printh__buf_print_set(buf_ptr, buf_count, expr), \
    (hwrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

#define str_print(string, expr) \
    (printh__str_print_set(string, expr), PRINTH__CTX_NAME.status)

#define str_print_wrote(string, wrote, expr) \
    (printh__str_print_set(string, expr), (wrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

/* Same print function but with automatic new line */

#define printh__append_newline() printh__ctx_string_len(&PRINTH__CTX_NAME, PRINT_NEW_LINE, 1)
#define file_println(file, expr) \
    (printh__file_print_set(file, expr) || printh__append_newline(), PRINTH__CTX_NAME.status)

#define file_println_wrote(file, wrote, expr) \
    (printh__file_print_set(file, expr) || printh__append_newline(),\
    (wrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

#define buf_println(buf_ptr, buf_count, expr) \
    (printh__buf_print_set(buf_ptr, buf_count, expr) || printh__append_newline(),\
    PRINTH__CTX_NAME.status)

#define buf_println_wrote(buf_ptr, buf_count, wrote, expr) \
    (printh__buf_print_set(buf_ptr, buf_count, expr) || printh__append_newline(), \
    (wrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

#define str_println(string, expr) \
    (printh__str_print_set(string, expr) || printh__append_newline(), PRINTH__CTX_NAME.status)

#define str_println_wrote(string, wrote, expr) \
    (printh__str_print_set(string, expr) || printh__append_newline(), \
    (wrote) = PRINTH__CTX_NAME.written, PRINTH__CTX_NAME.status)

/* Formater setters - User can make their own formats by passing function and variable names */
#define printh__print_format_1(func, var) \
   "") || func(&PRINTH__CTX_NAME, var) || printh__ctx_string(&PRINTH__CTX_NAME, ""

#define printh__print_format_2(func, var, var2) \
   "") || func(&PRINTH__CTX_NAME, var, var2) || printh__ctx_string(&PRINTH__CTX_NAME, ""

/* Public formats */
#define arg_fp(num) printh__print_format_1(printh__ctx_fp, num)
#define arg_hex(num) printh__print_format_1(printh__ctx_hex, num)
#define arg_oct(num) printh__print_format_1(printh__ctx_oct, num)
#define arg_dec(num) printh__print_format_1(printh__ctx_dec, num)
#define arg_str(str) printh__print_format_1(printh__ctx_string, str)
#define arg_str_len(str, len) printh__print_format_2(printh__ctx_string_len, str, len)
#define arg_str_lit(str) printh__print_format_2(printh__ctx_string_len, str, sizeof(str) - 1)

#endif /* PRINT_H */

#ifdef PRINT_IMPLEMENTATION

/* Note: Will be undef at the end */
#define printh__call_printf(count, buf, format, num) \
do { \
    printh__umax_t n = snprintf(buf, sizeof(buf), format, num); \
    if (count < 0) { return printh__state_stop; } \
    count = (printh__umax_t)n; \
    if (count >= sizeof(buf)) { count = sizeof(buf)-1; } \
} while(0);

/* Internal helpers */
#ifndef PRINT_USE_PRINTF
extern char* printh__format_base
(char* out_end, printh__umax_t num, printh__umax_t base);
#endif

#ifndef PRINT_USE_PRINTF_FOR_FP
extern char* printh__format_double
(char *end, double num);
#endif

#ifdef PRINT_DONT_USE_STRING_H
/* Implement needed internals */

void *printh__memcpy
(void *dst, const void *src, printh__umax_t n)
{
    char *d = dst;
    const char *s = src;

    while(n--) *d++ = *s++;
    return dst;
}

printh__umax_t printh__strlen
(const char *str)
{
    printh__umax_t n = 0;
    while(*str++) n++;
    return n;
}

#endif /* PRINT_USE_STRING_H */

printh__state_t printh__ctx_string_len
(printh__ctx_t *ctx, const char *str, printh__umax_t len)
{
    printh__umax_t wrote = 0, max = 0;

    switch (ctx->type) {
        case PRINTH__CTX_FILE: {
            wrote = PRINT_FILE_OUT(str, sizeof(char), len, ctx->target.file);
        } break;
        case PRINTH__CTX_STR: {
            wrote = PRINT_STRING_OUT(ctx->target.str, str, len);
        } break;
        case PRINTH__CTX_BUF: {
            max = (ctx->written < ctx->target.buf.count)
                ? ctx->target.buf.count - ctx->written : 0;
            max = (max > len) ? len : max;
            wrote = max;
            PRINT_MEMCPY((char *)ctx->target.buf.ptr + ctx->written, str, max);
        } break;
        default: { ctx->status = print_unknown_format; return printh__state_stop; }
    }

    ctx->written += wrote;
    ctx->status = (wrote == len) ? print_success : print_partial;
    return (ctx->status) ? printh__state_stop : printh__state_continue;
}

printh__state_t printh__ctx_hex
(printh__ctx_t *ctx, printh__umax_t num)
{
    char buf[PRINT_TMP_SIZE], *out = buf; printh__umax_t count = 0;

#ifdef PRINT_USE_PRINTF
    printh__call_printf(count, buf, PRINT_HEX_FORMAT, num);
#else
    out = printh__format_base(buf + PRINT_TMP_SIZE, num, 16);
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return printh__ctx_string_len(ctx, out, count);
}

printh__state_t _print_ctx_oct
(printh__ctx_t *ctx, printh__umax_t num)
{
    char buf[PRINT_TMP_SIZE], *out = buf; printh__umax_t count = 0;

#ifdef PRINT_USE_PRINTF
    printh__call_printf(count, buf, PRINT_OCT_FORMAT, num);
#else
    out = printh__format_base(buf + PRINT_TMP_SIZE, num, 8);
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return printh__ctx_string_len(ctx, out, count);
}

printh__state_t printh__ctx_dec
(printh__ctx_t *ctx, printh__max_t neg_num)
{
    char buf[PRINT_TMP_SIZE], *out = buf;
    printh__umax_t count = 0;

#ifdef PRINT_USE_PRINTF
    printh__call_printf(count, buf, PRINT_DEC_FORMAT, neg_num);
#else
    printh__umax_t num = 0;
    char is_negative = 0;

    /* Check for negative number */
    if(neg_num < 0) {
        is_negative = 1;
        num = (printh__umax_t)(-(neg_num + 1)) + 1;
    } else { num = (printh__umax_t)neg_num; }

    out = printh__format_base(buf + PRINT_TMP_SIZE, num, 10);

    if(is_negative) { *--out = '-'; }
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return printh__ctx_string_len(ctx, out, count);
}

printh__state_t printh__ctx_fp
(printh__ctx_t *ctx, double num)
{
    char buf[PRINT_TMP_SIZE], *out = buf;
    char is_negative = 0;
    printh__umax_t count = 0;

#ifdef PRINT_USE_PRINTF_FOR_FP
    printh__call_printf(count, buf, PRINT_FP_FORMAT, num);
#else
    /* Check for negative number */
    if(num < 0) { is_negative = 1; num *= -1; }
    out = printh__format_double(buf + PRINT_TMP_SIZE, num);
    if(is_negative) { *--out = '-'; }
    count = buf + PRINT_TMP_SIZE - out;
#endif

    return printh__ctx_string_len(ctx, out, count);
}

printh__state_t printh__ctx_string
(printh__ctx_t *ctx, const char *str)
{
    /* Skip if empty string like "" which saves little bit of time */
    /* This happens a lot because of conjunctions */
    if(str && !*str) { return printh__state_continue; }
    if(str) {
        return printh__ctx_string_len(ctx, str, PRINT_STRLEN(str));
    }
    return printh__ctx_string_len(ctx, PRINT_NULL_STRING, sizeof(PRINT_NULL_STRING) - 1);
}

/* TODO: This will be slow because we can not do masking for hex and oct
 *       so we need different function for them
 */
/* Note: num can not be negative and '-' should added afterwards by the caller */
/* Note: We expect caller to give enough buffer to us and pointer should be at
 *       the end of the buffer a normal integer representation wont pass ~25 characters
 *       and we are using 64(default one) so it should be good.
 */
char* printh__format_base
(char *end, printh__umax_t num, printh__umax_t base)
{
    static const char hex[] = PRINTH__HEX_TABLE;
    do {
        *--end = hex[num % base];
        num /= base;
    } while(num);
    return end;
}

/* Note: Dont use this function its very primitive */
char* printh__format_double(char *end, double num)
{
    printh__umax_t integer = 0;
    printh__umax_t fraction = 0;
    int i = 0;
    /* Cut -2 one for null termination one for indexing */
    printh__umax_t len = sizeof(PRINT_FLOAT_SEPARATOR) - 2;
    const char *sep = PRINT_FLOAT_SEPARATOR + len;

    integer = (printh__umax_t)num;
    num -= (double)integer;
    fraction = (printh__umax_t)(num * 1000000.0 + 0.5);

    /* Handle rounding overflow */
    if (fraction >= 1000000) {
        integer++;
        fraction -= 1000000;
    }

    for(i = 0; i < 6; i++) {
        *--end = '0' + (fraction % 10);
        fraction /= 10;
    }

    while((*--end = *sep--) && len--);

    return printh__format_base(end, integer, 10);
}

#undef printh__call_printf
#endif /* PRINT_IMPLEMENTATION */
