#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include "enum_codec.h"

void print_signature(const char* label, Signature* sig)
{
    printf("%s: ", label);
    if (!sig->paramCount)
        printf("(void) ");
    for (int i = 0; i < sig->paramCount; i++)
        printf("%d ", sig->params[i]);
    printf("->(%d)\n", (int)sig->returnType);
}
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
    case ENCODE_0: return VOID; 
    case ENCODE_I4: return I4; 
    case ENCODE_I8: return I8; 
    default: assert(false);
    }    
    return INVALID;
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
bool check_signatures(const Signature* a, const Signature* b)
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