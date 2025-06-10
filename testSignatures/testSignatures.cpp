// testSignatures.cpp : Defines the entry point for the application.
//
#include <assert.h>

#include "testSignatures.h"
#include "codec/enum_codec.h"
#include "printing/printing.h"
using namespace std;

int main()
{		
	InterpInternalSignature tests[]{
		{SIG_PARAM_VOID, 0, {}},
		{SIG_PARAM_VOID, 1, {SIG_PARAM_I8}},
		{SIG_PARAM_VOID, 1, {SIG_PARAM_I4}},
		{SIG_PARAM_I4, 1, {SIG_PARAM_I4}},
		{SIG_PARAM_I8, 1, {SIG_PARAM_I4}},
		{SIG_PARAM_VOID, 2, {SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_I4, 2, {SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_I8, 2, {SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_VOID, 3, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4}},
		{SIG_PARAM_I4, 3, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4}},
		{SIG_PARAM_I8, 3, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4}},
		{SIG_PARAM_VOID, 4, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4}},
		{SIG_PARAM_I4, 4, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4}},
		{SIG_PARAM_I8, 4, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4}},
		{SIG_PARAM_VOID, 5, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_I4, 5, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_I8, 5, {SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I4, SIG_PARAM_I8}},
		{SIG_PARAM_VOID, 6, {SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I8, 6, {SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I4, 6, {SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I8, 6, {SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I4, 6, {SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I8, 6, {SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_I4, 6, {SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I4, SIG_PARAM_I8, SIG_PARAM_I8}},
		{SIG_PARAM_INVALID}
	};
	int index = 0;	
	while (true)
	{
		InterpInternalSignature s_in = tests[index];
		if (s_in.returnType == SIG_PARAM_INVALID)
			break;
		InterpInternalSignature s_out{};
		print_signature("Signature_in", &s_in);
		uint16_t encoded = encode_signature(s_in.params, s_in.paramCount, s_in.returnType);
		printf("Got encoded value: %d\n", encoded);

		decode_signature(encoded, s_out.params, &s_out.paramCount, &s_out.returnType);
		print_signature("Signature_out", &s_out);
		assert(get_num_params(encoded) == s_out.paramCount);
		assert(get_return_type(encoded) == s_out.returnType);
		assert(check_signatures(&s_in, &s_out));
		index++;
	} 
	print_enums("mintSigs.h", "MINT_ICALLSIG", 6);
	print_function_calls("interp-icalls.c", "MINT_ICALLSIG", 6);
	return 0;
}
