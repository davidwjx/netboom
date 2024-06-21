/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   const_array_macro.h
 * Author: user
 *
 * Created on March 12, 2018, 4:02 PM
 */

#define PRIMITIVE_CAT(x, y) x ## y
#define CAT(x, y) PRIMITIVE_CAT(x, y)
#define CONST_ARRAY_2(start, step) (start), (start) + (step)
#define CONST_ARRAY_4(start, step) CONST_ARRAY_2((start), (step)), CONST_ARRAY_2((start) + 2*(step), (step))
#define CONST_ARRAY_8(start, step) CONST_ARRAY_4((start), (step)), CONST_ARRAY_4((start) + 4*(step), (step))
#define CONST_ARRAY_16(start, step) CONST_ARRAY_8((start), (step)), CONST_ARRAY_8((start) + 8*(step), (step))
#define CONST_ARRAY_32(start, step) CONST_ARRAY_16((start), (step)), CONST_ARRAY_16((start) + 16*(step), (step))
#define CONST_ARRAY_64(start, step) CONST_ARRAY_32((start), (step)), CONST_ARRAY_32((start) + 32*(step), (step))
#define CONST_ARRAY(start, step, len) CAT(CONST_ARRAY_, len)(start, step)
#define CAT2_ARRAY(start, step, len, stride) CONST_ARRAY(start, step, len), CONST_ARRAY((start) + (stride), step, len)
#define CAT4_ARRAY(start, step, len, stride) CAT2_ARRAY(start, step, len, stride), CAT2_ARRAY((start) + (stride) * 2, step, len, stride)
#define CAT8_ARRAY(start, step, len, stride) CAT4_ARRAY(start, step, len, stride), CAT4_ARRAY((start) + (stride) * 4, step, len, stride)
#define CAT16_ARRAY(start, step, len, stride) CAT8_ARRAY(start, step, len, stride), CAT8_ARRAY((start) + (stride) * 8, step, len, stride)
#define CAT32_ARRAY(start, step, len, stride) CAT16_ARRAY(start, step, len, stride), CAT16_ARRAY((start) + (stride) * 16, step, len, stride)

