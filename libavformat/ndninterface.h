//
// Created by hawkxiang on 3/23/17.
//

#ifndef CENTAUR_NDNINTERFACE_H
#define CENTAUR_NDNINTERFACE_H

#include "pipe.h"
#ifdef __cplusplus
extern "C"{
#endif
    void getbyname(const char* name, pipe_producer_t* pipe_pro);
#ifdef __cplusplus
};
#endif

#endif //CENTAUR_NDNINTERFACE_H
