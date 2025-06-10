#ifndef ENUM_PRINTING
#define ENUM_PRINTING
#include "../codec/enum_codec.h"
#ifdef __cplusplus
extern "C" {
#endif
	extern void print_enums(const char* filename, const char* prefix, int maxParamCount);
	extern void print_function_calls(const char* filename, const char* prefix, int maxParamCount);
	extern void print_signature(const char* label, InterpInternalSignature* sig);

#ifdef __cplusplus
}
#endif

#endif