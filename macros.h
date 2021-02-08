#ifndef EPDF_MACROS_H
#define EPDF_MACROS_H


#ifndef __has_attribute
#define __has_attribute(x) 0
#endif

#ifndef __has_builtin
#define __has_builtin(x) 0
#endif

#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define EPDF_GNUC_CHECK(maj, min)                                     \
    (((__GNUC__ << 20) + (__GNUC_MINOR__ << 10)) >= (((maj) << 20) + ((min) << 10)))
#else
#  define EPDF_GNUC_CHECK(maj, min) 0
#endif

#ifndef EPDF_PRINTF
# if EPDF_GNUC_CHECK(2, 5) || defined(__clang__)
#  define EPDF_PRINTF(format_idx, arg_idx)                          \
    __attribute__((__format__ (__printf__, format_idx, arg_idx)))
# else
#  define EPDF_PRINTF(format_idx, arg_idx)
# endif
#endif

#ifndef EPDF_UNUSED
# if defined(__GNUC__) || defined(__clang__)
#  define EPDF_UNUSED(x) UNUSED_ ## x __attribute__((unused))
# elif defined(__LCLINT__)
#  define EPDF_UNUSED(x) /*@unused@*/ x
# else
#  define EPDF_UNUSED(x) x
# endif
#endif

#ifndef EPDF_HIDDEN
# if EPDF_GNUC_CHECK(4, 0) || __has_attribute(visibility)
#  define EPDF_HIDDEN __attribute__((visibility("hidden")))
# elif defined(__SUNPRO_C)
#  define EPDF_HIDDEN __hidden
# else
#  define EPDF_HIDDEN
# endif
#endif

#ifndef EPDF_VISIBLE
# if EPDF_GNUC_CHECK(4, 0) || __has_attribute(visibility)
#  define EPDF_VISIBLE __attribute__((visibility("default")))
# else
#  define EPDF_VISIBLE
# endif
#endif

#ifndef EPDF_DEPRECATED
# if defined(__GNUC__) || __has_attribute(deprecated)
#  define EPDF_DEPRECATED(x) x __attribute__((deprecated))
#  define EPDF_DEPRECATED_ __attribute__((deprecated))
# else
#  define EPDF_DEPRECATED(x) x
#  define EPDF_DEPRECATED_
# endif
#endif

#ifndef EPDF_ALLOC_SIZE
# if (!defined(__clang__) && EPDF_GNUC_CHECK(4, 3)) ||      \
    (defined(__clang__) && __has_attribute(__alloc_size__))
#  define EPDF_ALLOC_SIZE(...) __attribute__((alloc_size(__VA_ARGS__)))
# else
#  define EPDF_ALLOC_SIZE(x)
# endif
#endif

#ifndef EPDF_DO_PRAGMA
# if defined(__GNUC__) || defined(__clang__)
#  define EPDF_DO_PRAGMA(x) _Pragma(#x)
# else
#  define EPDF_DO_PRAGMA(x)
# endif
#endif

#ifndef EPDF_IGNORE_DEPRECATED
# define EPDF_IGNORE_DEPRECATED                                         \
    EPDF_DO_PRAGMA(GCC diagnostic push)                                 \
    EPDF_DO_PRAGMA(GCC diagnostic ignored "-Wdeprecated-declarations")
#endif

#ifndef EPDF_UNIGNORE
# define EPDF_UNIGNORE                          \
    EPDF_DO_PRAGMA(GCC diagnostic pop)
#endif


#define UNUSED(x) EPDF_UNUSED(x)
#define DEPRECATED(x) EPDF_DEPRECATED(x)
#define EPDF_PLUGIN_API EPDF_VISIBLE

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#endif
