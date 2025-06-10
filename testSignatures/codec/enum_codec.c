#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "enum_codec.h"

#define ENCODE_0 0b00
#define ENCODE_I4 0b01
#define ENCODE_I8 0b10

int get_num_params(uint16_t encoded)
{
    int numParams = 0;
    for (int i = 0; i < MAX_SIG_PARAMS; ++i) {
        uint16_t code = (encoded >> (2 * (MAX_SIG_PARAMS - i))) & 0b11;
        if (code == ENCODE_0)
            break;
        numParams++;
    }
    return numParams;
}
param_type_e get_return_type(uint16_t encoded)
{
    uint16_t ret_code = encoded & 0b11;
    switch (ret_code)
    {
    case ENCODE_0: return SIG_PARAM_VOID;
    case ENCODE_I4: return SIG_PARAM_I4;
    case ENCODE_I8: return SIG_PARAM_I8;
    default: assert(false);
    }    
    return SIG_PARAM_INVALID;
}


/* encode with first parameter always shifted left 12 bits */
uint16_t encode_signature(const int* params, int param_count, int return_type) {
    uint16_t encoded = 0;

    for (int i = 0; i < param_count && i < MAX_SIG_PARAMS; ++i) {
        uint16_t code = (params[i] == 4) ? 0b01 : (params[i] == 8) ? 0b10 : 0b10;
        encoded |= (code << (2 * (MAX_SIG_PARAMS - i)));
    }

    // Encode return type in the last 2 bits
    uint16_t ret_code = (return_type == 4) ? 0b01 : (return_type == 8) ? 0b10 : 0b00;
    encoded |= ret_code;

    return encoded;
}

void decode_signature(uint16_t encoded, int* params_out, int* param_count_out, int* return_type_out) {
    int count = 0;

    for (int i = 0; i < MAX_SIG_PARAMS; ++i) {
        uint16_t code = (encoded >> (2 * (MAX_SIG_PARAMS - i))) & 0b11;
        if (code == 0b01) {
            params_out[count++] = 4;
        }
        else if (code == 0b10) {
            params_out[count++] = 8;
        }
        else {
            break; // Stop at first unused param
        }
    }

    *param_count_out = count;

    uint16_t ret_code = encoded & 0b11;
    if (ret_code == 0b01) {
        *return_type_out = 4;
    }
    else if (ret_code == 0b10) {
        *return_type_out = 8;
    }
    else {
        *return_type_out = 0;
    }
}
bool check_signatures(const InterpInternalSignature* a, const InterpInternalSignature* b)
{
    if (a->paramCount != b->paramCount)
        return false;
    if (a->returnType != b->returnType)
        return false;
    for (int i = 0; i < a->paramCount; i++)
    {
        if (a->params[i] != b->params[i])
            return false;
    }
    return true;
}

void check_signature(InterpInternalSignature* s_in)
{
    InterpInternalSignature s_out;
    uint16_t encoded = encode_signature(s_in->params, s_in->paramCount, s_in->returnType);
    decode_signature(encoded, s_out.params, &s_out.paramCount, &s_out.returnType);
    assert(get_num_params(encoded) == s_out.paramCount);
    assert(get_return_type(encoded) == s_out.returnType);
    assert(check_signatures(s_in, &s_out));
}




