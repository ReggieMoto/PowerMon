/* ============================================================== */
/*
 * sha3c.c
 *
 * Copyright (c) 2015 Andrey Jivsov. crypto@brainhub.org
 * All Rights Reserved.
 */
/* ============================================================== */
/*
 * NOTICE:  This code is release into the public domain.
 *
 * Based on code from http://keccak.noekeon.org/ .
 *
 */
/* ============================================================== */

#ifndef __SHA3C_H__
#define __SHA3C_H__

typedef unsigned long uint64_t;
typedef unsigned char uint8_t;

#ifdef SHA_DEBUG_POWERMON
#undef SHA_DEBUG_POWERMON
#endif
#define SHA_DEBUG_POWERMON 0

/* 'Words' here refers to uint64_t */
#define SHA3_KECCAK_SPONGE_WORDS \
	(((1600)/8/*bits to byte*/)/sizeof(uint64_t))
typedef struct sha3_context_ {
    uint64_t saved;             /* the portion of the input message that we
                                 * didn't consume yet */
    union {                     /* Keccak's state */
        uint64_t s[SHA3_KECCAK_SPONGE_WORDS];
        uint8_t sb[SHA3_KECCAK_SPONGE_WORDS * 8];
    };
    unsigned byteIndex;         /* 0..7--the next byte after the set one
                                 * (starts from 0; 0--none are buffered) */
    unsigned wordIndex;         /* 0..24--the next word to integrate input
                                 * (starts from 0) */
    unsigned capacityWords;     /* the double size of the hash output in
                                 * words (e.g. 16 for Keccak 512) */
} sha3_context;

#ifndef SHA3_ROTL64
#define SHA3_ROTL64(x, y) \
	(((x) << (y)) | ((x) >> ((sizeof(uint64_t)*8) - (y))))
#endif

extern void sha3_Init256(void *priv);
extern void sha3_Update(void *priv, void const *bufIn, size_t len);
extern void const * sha3_Finalize(void *priv);

#endif /* __SHA3C_H__ */
