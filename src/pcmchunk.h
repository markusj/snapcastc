#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

// CHUNK_HEADER_SIZE defines the offset at which data will be written in a AUDIO packet and the size of the PCM header without the pointer to the data
#define CHUNK_HEADER_SIZE ( 17 )

enum { CODEC_PCM = 0, CODEC_OPUS = 1 };

typedef struct {
	uint32_t play_at_tv_sec;
	uint32_t play_at_tv_nsec;
	uint32_t samples;
	uint8_t frame_size;
	uint8_t channels;
	uint16_t size;
	uint8_t codec;
	uint8_t *data;
} pcmChunk;

void chunk_copy_meta(pcmChunk *dest, const pcmChunk *src);
void get_emptychunk(pcmChunk *ret, unsigned int length_ms);
bool chunk_is_empty(pcmChunk *c);
void chunk_hton(pcmChunk *chunk);
void chunk_ntoh(pcmChunk *chunk);

int chunk_getduration_ms(pcmChunk *chunk);

bool chunk_decode(pcmChunk *chunk);
struct timespec chunk_get_play_at(pcmChunk *chunk);
int chunk_cmp(pcmChunk *c2, pcmChunk *c1);

void pcmchunk_shaveoff(pcmChunk *chunk, int frames);
void chunk_free_members(pcmChunk *chunk);
