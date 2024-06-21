#ifndef INUITIVE_CNN_H
#define INUITIVE_CNN_H

#ifdef __cplusplus
extern "C" {
#endif

struct GraphParams
{
    int argc;
    char** argv;
};

void* run_inu_graph(void* params);

#ifdef __cplusplus
}
#endif

#endif

