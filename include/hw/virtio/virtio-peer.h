/*
 * Virtio PEER Support
 *
 * Copyright Huawei Technologies
 * Copyright Ajo Jose Panoor<ajo.jose.panoor@huawei.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.  See the COPYING file in the
 * top-level directory.
 */

#ifndef _QEMU_VIRTIO_PEER_H
#define _QEMU_VIRTIO_PEER_H

#include "sysemu/peer.h"
#include "standard-headers/linux/virtio_peer.h"

#define TYPE_VIRTIO_PEER "virtio-peer-device"
#define VIRTIO_PEER(obj) \
        OBJECT_CHECK(VirtIOPeer, (obj), TYPE_VIRTIO_PEER)
#define VIRTIO_PEER_GET_PARENT_CLASS(obj) \
        OBJECT_GET_PARENT_CLASS(obj, TYPE_VIRTIO_PEER)

typedef struct VirtIOPeerArgs {
    char *cmd_size;
    char *cmd_role;
}VirtIOPeerArgs;

typedef struct VirtIOWinCfg {
    int fd;
    int bar;
    int flags;
    void *va;
    uint32_t shmid;
    char shm_name[10];
    uint32_t win_size;
    MemoryRegion wmr;
}VirtIOWinCfg;

typedef struct VirtIOPeer {
    VirtIODevice parent_obj;
    int role;
    size_t dev_cfg_size;
    PeerBackend *peer;
    VirtQueue *rx_vq;
    VirtQueue *tx_vq;
    VirtIOPeerArgs args;
    VirtIOWinCfg win_cfg[2];
    struct virtio_peer_config dev_cfg;
    struct virtio_window_config win_common_cfg;
}VirtIOPeer;

#endif
