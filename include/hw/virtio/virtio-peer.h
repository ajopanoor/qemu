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

typedef struct VirtIOPeerConf {
    PeerBackend *peer;

    uint32_t win_size;

}VirtIOPeerConf;

typedef struct VirtIOPeer {
    VirtIODevice parent_obj;

    VirtQueue *rx_vq;
    VirtQueue *tx_vq;

    VirtIOPeerConf conf;

} VirtIOPeer;

#endif
