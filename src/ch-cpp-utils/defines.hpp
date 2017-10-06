/*
 * defines.hpp
 *
 *  Created on: Oct 5, 2017
 *      Author: corehacker
 */

#ifndef SRC_CH_CPP_UTILS_DEFINES_HPP_
#define SRC_CH_CPP_UTILS_DEFINES_HPP_


#define SAFE_DELETE(pointer) \
   do { \
      if(pointer) { \
         delete pointer; \
         pointer = NULL; \
      } \
   } while(0)

#define SAFE_DELETE_RO(pointer) \
   do { \
      if(pointer) { \
         delete pointer; \
      } \
   } while(0)


#endif /* SRC_CH_CPP_UTILS_DEFINES_HPP_ */
