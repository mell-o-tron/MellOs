//
// Created by matias on 10/1/25.
//

#ifndef MELLOS_INIT_H
#define MELLOS_INIT_H
#include "stdint.h"

void init_assertions(void (*clscolorptr)(uint8_t), void (*set_cursor_pos_rawptr)(uint16_t),
    void (*kclear_screenptr)(void));

#endif //MELLOS_INIT_H