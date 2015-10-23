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
#define VIRTIO_PCI_WINDOW_RO_SIZE   0
#define VIRTIO_PCI_WINDOW_RW_SIZE   4
#define VIRTIO_PCI_WINDOW_RO_BAR    8
#define VIRTIO_PCI_WINDOW_RW_BAR    9

struct virtio_peer_config {
    uint64_t queue_data_offset;
    uint32_t queue_data_size;
    uint32_t queue_magic;
    uint8_t queue_flags;
    uint8_t queue_window_idr;
    uint8_t queue_window_idw;
} QEMU_PACKED;

struct virtio_window_config {
    uint32_t ro_win_size;
    uint32_t rw_win_size;
    uint8_t rw_bar;
    uint8_t ro_bar;
} QEMU_PACKED;
#endif /* _LINUX_VIRTIO_PEER_H */
