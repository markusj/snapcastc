#pragma once

#include "clientmgr.h"
#include "pcmchunk.h"
#include "taskqueue.h"
#include "vector.h"

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define PACKET_FORMAT_VERSION 1
#define CLIENTNAME_MAXLEN

enum { CLIENT_OPERATION, AUDIO_DATA, SERVER_OPERATION };  //
enum { REQUEST, HELLO };				  // TLV - types for client operations
enum { STREAM_INFO };					  // TLV - server operations
enum { AUDIO_PCM, AUDIO_OPUS, AUDIO_OGG, AUDIO_FLAC };    // TLV type for codec

typedef struct __attribute__((__packed__)) {
	uint8_t version;
	uint8_t type;
	uint16_t empty;
	uint32_t nonce;
} intercom_packet_hdr;

typedef struct __attribute__((__packed__)) {
	uint8_t type;
	uint8_t length;
	// after this a dynamic buffer is appended to hold the client name
} tlv_hello;

typedef struct __attribute__((__packed__)) {
	intercom_packet_hdr hdr;
	// after this a dynamic buffer is appended to hold TLV
} intercom_packet_hello;

typedef struct __attribute__((__packed__)) {
	intercom_packet_hdr hdr;
	// after this a dynamic buffer is appended to hold TLV.
} intercom_packet_op;

typedef struct __attribute__((__packed__)) {
	intercom_packet_hdr hdr;
	// after this a dynamic buffer is appended to hold TLV.
} intercom_packet_sop;

typedef struct __attribute__((__packed__)) {
	intercom_packet_hdr hdr;
	// after this a dynamic buffer is appended to hold TLV.
} intercom_packet_audio;

typedef VECTOR(client_t) client_v;

typedef struct __attribute__((__packed__)) {
	uint8_t type;
	uint8_t length;
	uint16_t empty;
	// 	uint32_t seqno;
} intercom_packet_tlv;

struct intercom_task {
	uint16_t packet_len;
	uint8_t *packet;
	struct in6_addr *recipient;
	taskqueue_t *check_task;
	uint8_t retries_left;
};

typedef struct {
	struct in6_addr serverip;
	struct snaptx *snapctx;
	VECTOR(intercom_packet_hdr) recent_packets;
	int fd;
	uint16_t port;
	uint16_t serverport;
	uint32_t nodeid;
	int mtu;

	size_t bufferwindex;
	size_t buffer_elements;
	size_t bufferrindex;
	size_t lastreceviedseqno;
	int buffer_wraparound;
	void *buffer;

} intercom_ctx;

void intercom_send_audio(intercom_ctx *ctx, pcmChunk *chunk);
void intercom_recently_seen_add(intercom_ctx *ctx, intercom_packet_hdr *hdr);
void intercom_send_packet(intercom_ctx *ctx, uint8_t *packet, ssize_t packet_len);
void intercom_seek(intercom_ctx *ctx, const struct in6_addr *address);
void intercom_init_unicast(intercom_ctx *ctx);
void intercom_init(intercom_ctx *ctx);
void intercom_handle_in(intercom_ctx *ctx, int fd);
bool intercom_hello(intercom_ctx *ctx, const struct in6_addr *recipient, int port);
struct timespec intercom_get_time_next_audiochunk(intercom_ctx *ctx);

pcmChunk intercom_getnextaudiochunk(intercom_ctx *ctx);