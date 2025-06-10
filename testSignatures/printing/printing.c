#include "printing.h"
#include <stdio.h>

typedef void (*printFn)(FILE*, const char*, InterpInternalSignature*);

void print_signature(const char* label, InterpInternalSignature* sig)
{
    printf("%s: ", label);
    if (!sig->paramCount)
        printf("(void) ");
    for (int i = 0; i < sig->paramCount; i++)
        printf("%d ", sig->params[i]);
    printf("->(%d)\n", (int)sig->returnType);
}

/* output code to typedef a function pointer, and call it using the inputs to do_icall in interp.c */
static void fprint_signature_fn_call(FILE* out, InterpInternalSignature* sig)
{
    fprintf(out, "\ttypedef ");
    // return type
    if (sig->returnType == VOID)
        fprintf(out, "void ");
    else
        fprintf(out, "%s ", sig->returnType == I8 ? "I8" : "I4");
    fprintf(out, "(*T)(");
    // parameter types
    if (!sig->paramCount)
        fprintf(out, "void");
    for (int i = 0; i < sig->paramCount; i++)
    {
        fprintf(out, "%s", sig->params[i] == I8 ? "I8" : "I4");
        if (i < sig->paramCount - 1)
            fprintf(out, ",");
    }
    fprintf(out, ");\n");
    // cast provided ptr to this function pointer
    fprintf(out, "\tT func = (T)ptr;\n");
    // call the function
    if (sig->returnType == VOID)
        fprintf(out, "\tfunc (");
    else
        fprintf(out, "\tret_sp->data.p = %sfunc (", sig->returnType == I8 ? "" : "(I8)");
    // params
    for (int i = 0; i < sig->paramCount; i++)
    {
        fprintf(out, "%ssp[%d].data.p", sig->params[i] == I8 ? "" : "(I4)", i);
        if (i < sig->paramCount - 1)
            fprintf(out, ",");
    }
    // end of function call
    fprintf(out, ");\n");
}

/* print a signature in an enum declaration friendly manner*/
static void fprint_signature_enum(FILE* out, const char* prefix, InterpInternalSignature* sig)
{
    uint16_t encoded = encode_signature(sig->params, sig->paramCount, sig->returnType);
    fprintf(out, "%s_", prefix);
    if (!sig->paramCount)
        fprintf(out, "V");
    for (int i = 0; i < sig->paramCount; i++)
        fprintf(out, "%d", sig->params[i]);
    if (sig->returnType == VOID)
        fprintf(out, "_V");
    else
        fprintf(out, "_%d", sig->returnType);
    fprintf(out, " = %d,\n", encoded);
}

static void fprint_signature_case(FILE* out, const char* prefix, InterpInternalSignature* sig)
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

void fprint_signature_case_with_fn_call(FILE* out, const char* prefix, InterpInternalSignature* sig)
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
    fprintf(out, ": {\n");
    fprint_signature_fn_call(out, sig);
    fprintf(out, "\tbreak;\n");
    fprintf(out, "};\n");
}

void fprint_enums_for_param_count(FILE* out, const char* prefix, int paramCount, printFn printFunction)
{
    // for each permutation of 4 or 8 byte parameters output a VOID, 4 and 8 output 
    int perms = 1 << paramCount; // 2^X permutations
    InterpInternalSignature sigs[3];
    for (int i = 0; i < 3; i++)
        sigs[i].paramCount = paramCount;
    sigs[0].returnType = VOID;
    sigs[1].returnType = I4;
    sigs[2].returnType = I8;
    for (int sigIndex = 0; sigIndex < 3; sigIndex++)
    {
        InterpInternalSignature* sig = &(sigs[sigIndex]);
        /* the set of integers from 0 to numPerms contains the bitfields with each permutation of 1 and 0 up to that numPerms. */
        for (int i = 0; i < perms; ++i) {
            for (int j = 0; j < paramCount; ++j) {
                sig->params[j] = (i & (1 << (paramCount - j - 1))) ? 8 : 4;
            }
            printFunction(out, prefix, sig);
            check_signature(sig);
        }
    }
}


void print_enums(const char* filename, const char* prefix, int maxParamCount)
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

void print_function_calls(const char* filename, const char* prefix, int maxParamCount)
{
    FILE* out = fopen(filename, "w");
    if (!out)
        return;
    fprintf(out, "switch (op) {\n");
    for (int i = 0; i <= maxParamCount; i++)
    {
        fprint_enums_for_param_count(out, prefix, i, fprint_signature_case_with_fn_call);
    }
    fprintf(out, "default:\n");
    fprintf(out, "    g_assert_not_reached();\n");
    fprintf(out, "}\n");

    fclose(out);
}
