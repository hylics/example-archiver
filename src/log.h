/*
 * log.h
 *
 *  Created on: Jul 21, 2019
 *      Author: md761
 */

#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

#ifndef NDEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif


#endif /* LOG_H_ */
