#ifndef PHP_TIMEOUT_H
#define PHP_TIMEOUT_H
#endif

#define PHP_TIMEOUT_VERSION "0.1"

#if defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_TIMEOUT_API __attribute__ ((visibility("default")))
#endif

#define TT_CALL_FUNCTION_SUCC	0
#define TT_SET_FUNCTION_CALL_INFO_ERROR	100
#define TT_FUNCTION_CALL_FAILURE	101
#define TT_SET_SIGNAL_ERROR	102
#define TT_SET_TIMER_ERROR	103
#define TT_CALL_FUNCTION_TIMEOUT	104
