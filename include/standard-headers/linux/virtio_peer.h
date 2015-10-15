#ifndef _LINUX_VIRTIO_PEER_H
#define _LINUX_VIRTIO_PEER_H
/* This header is BSD licensed so anyone can use the definitions to implement
 * compatible drivers/servers. */
#include "standard-headers/linux/virtio_ids.h"
#include "standard-headers/linux/virtio_config.h"

#define VIRTIO_PEER_FLAGS_REMOTE	0x1
#define VIRTIO_PEER_FLAGS_LOCAL		0x2

struct virtio_peer_config {
	uint64_t queue_data_offset;
	uint32_t queue_data_size;
	uint8_t queue_flags;
	uint8_t queue_window_idr;
	uint8_t queue_window_idw;
} QEMU_PACKED;

#endif /* _LINUX_VIRTIO_PEER_H */
