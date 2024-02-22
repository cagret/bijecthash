// inthash.h
#ifndef INTHASH_H
#define INTHASH_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint64_t hash_64(uint64_t key, uint64_t mask);
uint64_t hash_64i(uint64_t key, uint64_t mask);

#ifdef __cplusplus
}
#endif

#endif // INTHASH_H

