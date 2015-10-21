#ifndef _LINUX_VIRTIO_PEER_H
#define _LINUX_VIRTIO_PEER_H
/* This header is BSD licensed so anyone can use the definitions to implement
 * compatible drivers/servers. */
#include "standard-headers/linux/virtio_ids.h"
#include "standard-headers/linux/virtio_config.h"

#define VIRTIO_PEER_FLAGS_REMOTE    0x1
#define VIRTIO_PEER_FLAGS_LOCAL     0x2
#define VIRTIO_PEER_MAGIC           0x70656572
#define MASTER                      0
#define PEER                        1

struct virtio_window {
    uint64_t win_phy_start;
    uint32_t win_size;
} QEMU_PACKED;

struct virtio_peer_config {
    uint64_t queue_data_offset;
    uint32_t queue_data_size;
    uint32_t queue_magic;
    uint8_t queue_flags;
    uint8_t queue_window_idr;
    uint8_t queue_window_idw;
    uint8_t pad;
    struct virtio_window windows[2];
} QEMU_PACKED;

#endif /* _LINUX_VIRTIO_PEER_H */
