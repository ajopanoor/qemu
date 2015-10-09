/*
 * A virtio device implementing virtio based shared memory communication
 * between guests.
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * (at your option) any later version.  See the COPYING file in the
 * top-level directory.
 */

#include "qemu/iov.h"
#include "hw/qdev.h"
#include "hw/virtio/virtio.h"
#include "hw/virtio/virtio-peer.h"
#include "qom/object_interfaces.h"
#include "trace.h"

#define DEBUG_VIRTIO_PEER

#ifdef DEBUG_VIRTIO_PEER
#define DPRINTF(fmt, ...) \
do { printf("virtio_peer: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

static uint64_t get_features(VirtIODevice *vdev, uint64_t f, Error **errp)
{
    DPRINTF("%s\n", __func__);
    return f;
}

static void virtio_peer_handle_rx(VirtIODevice *vdev, VirtQueue *vq)
{
    DPRINTF("%s\n", __func__);
}

static void virtio_peer_handle_tx(VirtIODevice *vdev, VirtQueue *vq)
{
    DPRINTF("%s\n", __func__);
}

static void virtio_peer_device_realize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOPeer *vpeer = VIRTIO_PEER(dev);

    DPRINTF("%s\n", __func__);

    virtio_init(vdev, "virtio-peer", VIRTIO_ID_PEER, 0);

    vpeer->rx_vq = virtio_add_queue(vdev, 64, virtio_peer_handle_rx);

    vpeer->tx_vq = virtio_add_queue(vdev, 64, virtio_peer_handle_tx);
}

static void virtio_peer_device_unrealize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);

    DPRINTF("%s\n", __func__);

    virtio_cleanup(vdev);
}

static Property virtio_peer_properties[] = {
    DEFINE_PROP_END_OF_LIST(),
};

static void virtio_peer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    DPRINTF("%s\n", __func__);
    dc->props = virtio_peer_properties;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    vdc->realize = virtio_peer_device_realize;
    vdc->unrealize = virtio_peer_device_unrealize;
    vdc->get_features = get_features;
}

static void virtio_peer_initfn(Object *obj)
{
    VirtIOPeer *vpeer = VIRTIO_PEER(obj);

    DPRINTF("%s\n", __func__);
    object_property_add_link(obj, "peer", TYPE_PEER_BACKEND,
                             (Object **)&vpeer->conf.peer,
                             qdev_prop_allow_set_link_before_realize,
                             OBJ_PROP_LINK_UNREF_ON_RELEASE, NULL);
}

static const TypeInfo virtio_peer_info = {
    .name = TYPE_VIRTIO_PEER,
    .parent = TYPE_VIRTIO_DEVICE,
    .instance_size = sizeof(VirtIOPeer),
    .instance_init = virtio_peer_initfn,
    .class_init = virtio_peer_class_init,
};

static void virtio_register_types(void)
{
    DPRINTF("%s\n", __func__);
    type_register_static(&virtio_peer_info);
}

type_init(virtio_register_types)
