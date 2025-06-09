#ifndef ENUM_CODEC_
#define ENUM_CODEC_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_SIG_PARAMS 6
#ifndef bool
#define bool int
#endif 
#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#define ENCODE_0 0b00
#define ENCODE_I4 0b01
#define ENCODE_I8 0b10

	typedef enum {
		VOID = 0,
		I4 = 4,
		I8 = 8,
		INVALID = 666
	} param_type_e;

	typedef struct
	{
		int returnType;
		int paramCount;		
		int params[MAX_SIG_PARAMS];
	} Signature;

	extern uint16_t encode_signature(const int* params, int param_count, int return_type);
	extern void decode_signature(uint16_t encoded, int* params_out, int* param_count_out, int* return_type_out);
	extern void print_signature(const char* label, Signature* sig);
	extern bool check_signatures(const Signature* a, const Signature* b);
	extern param_type_e get_return_type(uint16_t encoded);
	extern int get_num_params(uint16_t encoded);
#ifdef __cplusplus
}
#endif

#endif // !_ENUM_CODEC__
