/*
 * utils.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <assert.h>

/* from kernel */
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

/* Usage of lambda macros: function or macros which take
 * function pointer as argument for function:

void func(int nb, void (*f)(int))
{
    int i;
    for (i = 0; i < nb; i++) f(i);
}

int main()
{
   func(4, LAMBDA(void _(int v) {
        printf("%d\n", v);
   }));
}

 * use "_" as name for lambda like a naming convention.
 * The lambda is only valid during the scope of the calling function,
 * so we have to be careful not to use it for asynchronous callbacks */
#define LAMBDA(c_) ({ c_ _;})

/** @brief The cleanup attribute runs a function when the variable goes out of scope.
 *  This attribute can only be applied to auto function scope variables;
 *  it may not be applied to parameters or variables with static storage duration.
 *  The function must take one parameter, a pointer to a type compatible with the variable.
 *  The return value of the function (if any) is ignored.
 *  If -fexceptions is enabled, then cleanup_function is run during the stack unwinding
 *  that happens during the processing of the exception. Note that the cleanup attribute
 *  does not allow the exception to be caught, only to perform an action.
 *  It is undefined what happens if cleanup_function does not return normally.
 */
#define AUTO_FREE(a_free_func) __attribute__((cleanup(a_free_func)))

#define UNUSED_ATTR __attribute__((__unused__))

#ifndef max
#define max(a, b)		\
	({ typeof (a) _a = (a);	\
	typeof (b) _b = (b);	\
	_a > _b ? _a : _b; })
#endif

#ifndef min
#define min(a, b)		\
	({ typeof (a) _a = (a);	\
	typeof (b) _b = (b);	\
	_a < _b ? _a : _b; })
#endif

#ifndef NDEBUG
#define my_assert(...) assert(__VA_ARGS__)
#else
#define my_assert()
#endif

#endif /* UTILS_H_ */
