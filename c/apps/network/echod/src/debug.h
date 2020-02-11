#ifndef _DEBUG_H_INCLUDED_
#define _DEBUG_H_INCLUDED_ 1

#if defined(M_HAS_BUILTIN)
#undef M_HAS_BUILTIN
#endif
#if defined(__has_builtin)
#define M_HAS_BUILTIN(builtin) __has_builtin(builtin)
#else
#define M_HAS_BUILTIN(builtin) (0)
#endif

/*
 * Branch prediction hints.
 */
#if __GNUC__ >= 3
# define M_UNLIKELY(exp) __builtin_expect ((exp), 0)
# define M_LIKELY(exp)   __builtin_expect ((exp), 1)
#else
# define M_UNLIKELY(exp) (exp)
# define M_LIKELY(exp)   (exp)
#endif

#define M_STRINGIFY_(x) #x
#define M_STRINGIFY(x)  M_STRINGIFY_(x)

#ifdef  NDEBUG

#define M_ASSERT(cond) ((void) 0)

#else

void __assertion_failed(char const *, char const *, int);

#define M_ASSERT(cond) \
  ((void) ((cond) || (__assertion_failed(#cond, __FILE__, __LINE__), 0)))

#endif  /* NDEBUG */

#endif  /* !defined(_DEBUG_H_INCLUDED_) */
