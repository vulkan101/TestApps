// testSignatures.cpp : Defines the entry point for the application.
//

#include "testSignatures.h"
#include "codec/enum_codec.h"
#include <assert.h>

using namespace std;

int main()
{		
	Signature tests[]{
		{VOID, 0, {}},
		{VOID, 1, {I8}},
		{VOID, 1, {I4}},
		{I4, 1, {I4}},
		{I8, 1, {I4}},
		{VOID, 2, {I4, I8}},
		{I4, 2, {I4, I8}},
		{I8, 2, {I4, I8}},
		{VOID, 3, {I4, I8, I4}},
		{I4, 3, {I4, I8, I4}},
		{I8, 3, {I4, I8, I4}},
		{VOID, 4, {I4, I8, I4, I4}},
		{I4, 4, {I4, I8, I4, I4}},
		{I8, 4, {I4, I8, I4, I4}},
		{VOID, 5, {I4, I8, I4, I4, I8}},
		{I4, 5, {I4, I8, I4, I4, I8}},
		{I8, 5, {I4, I8, I4, I4, I8}},
		{VOID, 6, {I8, I8, I8, I8, I8, I8}},
		{I8, 6, {I8, I8, I8, I8, I8, I8}},
		{I4, 6, {I8, I8, I8, I8, I8, I8}},
		{I8, 6, {I8, I8, I8, I8, I8, I8}},
		{I4, 6, {I8, I4, I8, I4, I8, I8}},
		{I8, 6, {I8, I4, I8, I4, I8, I8}},
		{I4, 6, {I8, I4, I8, I4, I8, I8}},

		{INVALID}
	};
	int index = 0;	
	while (true)
	{
		Signature s_in = tests[index];
		if (s_in.returnType == INVALID)
			break;
		Signature s_out{};
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
	return 0;
}
