#ifndef DECOMP_H
#define DECOMP_H

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include "compresse.h"

uint64_t decompress(FILE* encoded_file, FILE* file);




#endif
