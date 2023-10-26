//
// Created by hanyuan on 2023/10/25.
//

#ifndef TEMU_MACHINE_H
#define TEMU_MACHINE_H

#include "cpu.h"

class machine : public cpu {
public:
    machine():cpu(64,32){

    }
};


#endif //TEMU_MACHINE_H
