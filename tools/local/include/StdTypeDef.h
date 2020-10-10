/**
 * @file StdTypeDef.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2020-10-03
 * 
 * @copyright Copyright (c) 2020
 * 
 */


#ifndef __STDTYPEDEF_H__
#define __STDTYPEDEF_H__



#ifndef uint8
typedef unsigned char uint8;
#endif

#ifndef sint8
typedef signed char sint8;
#endif

#ifndef uint16
typedef unsigned short uint16;
#endif

#ifndef sint16
typedef signed short sint16;
#endif

#ifndef uint32
typedef unsigned int uint32;
#endif

#ifndef sint32
typedef signed int sint32;
#endif

#ifndef boolean
typedef unsigned char boolean;
#endif 

#ifndef FALSE
#define FALSE (0)
#endif

#ifndef TRUE
#define TRUE (!FALSE)
#endif
#endif