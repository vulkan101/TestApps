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

/* print a signature in an enum declaration friendly manner*/
void fprint_signature_enum(FILE* out, const char* prefix, Signature* sig)
{
    uint16_t encoded = encode_signature(sig->params, sig->paramCount, sig->returnType);
    fprintf(out, "%s_", prefix);
    if(!sig->paramCount)
        fprintf(out, "V");
    for (int i = 0; i < sig->paramCount; i++)
        fprintf(out, "%d", sig->params[i]);
    if(sig->returnType == VOID)
        fprintf(out, "_V");
    else 
        fprintf(out, "_%d", sig->returnType);
    fprintf(out, " = %d,\n", encoded);
}

void fprint_signature_case(FILE* out, const char* prefix, Signature* sig)
{
    uint16_t encoded = encode_signature(sig->params, sig->paramCount, sig->returnType);
    fprintf(out, "case %s_", prefix);
    if (!sig->paramCount)
        fprintf(out, "V");
    for (int i = 0; i < sig->paramCount; i++)
        fprintf(out, "%d", sig->params[i]);
    if (sig->returnType == VOID)
        fprintf(out, "_V");
    else
        fprintf(out, "_%d", sig->returnType);
    fprintf(out, ":\n");
}

static void CheckSignature(Signature* s_in)
{
    Signature s_out;
    uint16_t encoded = encode_signature(s_in->params, s_in->paramCount, s_in->returnType);
    decode_signature(encoded, s_out.params, &s_out.paramCount, &s_out.returnType);
    assert(get_num_params(encoded) == s_out.paramCount);
    assert(get_return_type(encoded) == s_out.returnType);
    assert(check_signatures(s_in, &s_out));
}

typedef void (*printFn)(FILE*, const char*, Signature*);

void fprint_enums_for_param_count(FILE* out, const char* prefix, int paramCount, printFn printFunction)
{    
    // for each permutation of 4 or 8 byte parameters output a VOID, 4 and 8 output 
    int perms = 1 << paramCount; // 2^X permutations
    Signature sigs[3];
    for (int i = 0; i < 3; i++)
        sigs[i].paramCount = paramCount;
    sigs[0].returnType = VOID;
    sigs[1].returnType = I4;
    sigs[2].returnType = I8;
    for (int sigIndex = 0; sigIndex < 3; sigIndex++)
    {
        Signature* sig = &(sigs[sigIndex]);
        /* the set of integers from 0 to numPerms contains the bitfields with each permutation of 1 and 0 up to that numPerms. */
        for (int i = 0; i < perms; ++i) {
            for (int j = 0; j < paramCount; ++j) {
                sig->params[j] = (i & (1 << (paramCount - j - 1))) ? 8 : 4;
            }                                      
            printFunction(out, prefix, sig);
            CheckSignature(sig);
        }        
    }
}


void PrintEnums(const char* filename, const char* prefix, int maxParamCount)
{
    FILE* out = fopen(filename, "w");
    if (!out)
        return;
    fprintf(out, "ENUM_DEFINITION {\n");
    for (int i = 0; i <= maxParamCount; i++)
    {
        fprint_enums_for_param_count(out, prefix, i, fprint_signature_enum);
    }
    fprintf(out, "}\n");
    fprintf(out, "// useful for switch statements\n");
    fprintf(out, "{\n");
    for (int i = 0; i <= maxParamCount; i++)
    {
        fprint_enums_for_param_count(out, prefix, i, fprint_signature_case);
    }
    fprintf(out, "}\n");
    fclose(out);
}