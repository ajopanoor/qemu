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
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define DEBUG_VIRTIO_PEER

#ifdef DEBUG_VIRTIO_PEER
#define DPRINTF(fmt, ...) \
do { printf("virtio_peer: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

static uint64_t virtio_peer_get_features(VirtIODevice *vdev, uint64_t f, Error **errp)
{
    DPRINTF("%s\n", __func__);
    return f;
}

static void virtio_peer_set_features(VirtIODevice *vdev, uint64_t features)
{
    DPRINTF("%s\n", __func__);
}

static void virtio_peer_handle_rx(VirtIODevice *vdev, VirtQueue *vq)
{
    DPRINTF("%s\n", __func__);
}

static void virtio_peer_handle_tx(VirtIODevice *vdev, VirtQueue *vq)
{
    DPRINTF("%s\n", __func__);
}

static void virtio_peer_get_config(VirtIODevice *vdev, uint8_t *config)
{
    VirtIOPeer *vpeer = VIRTIO_PEER(vdev);

    DPRINTF("%s\n", __func__);
    memcpy(config, &vpeer->dev_cfg, vpeer->dev_cfg_size);
}

static void virtio_peer_set_config(VirtIODevice *vdev, const uint8_t *config)
{
    VirtIOPeer *vpeer = VIRTIO_PEER(vdev);

    DPRINTF("%s\n", __func__);
    memcpy(&vpeer->dev_cfg, config, vpeer->dev_cfg_size);
}

static int config_window(VirtIOPeer *vpeer, int type)
{
    VirtIOWinCfg *win = &vpeer->win_cfg[type];
    char shm_name[2][10] = { "vpeer-ro", "vpeer-rw" };
    int ret = -ENOMEM;

    strncpy(win->shm_name, shm_name[type], 10);

    if ((win->fd = shm_open(win->shm_name, O_CREAT|O_RDWR|O_EXCL,
                    S_IRWXU|S_IRWXG|S_IRWXO)) > 0) {

        if (ftruncate(win->fd, win->win_size) != 0)
            DPRINTF("%s window %s shm_open failed\n", __func__, win->shm_name);

    } else if ((win->fd = shm_open(win->shm_name, O_CREAT|O_RDWR,
                        S_IRWXU|S_IRWXG|S_IRWXO)) < 0) {
        DPRINTF("%s window %s shm_open failed\n", __func__, win->shm_name);
        return ret;
    }

    win->va = mmap(0, win->win_size, PROT_READ|PROT_WRITE, MAP_SHARED,
                                                                win->fd, 0);
    if(!win->va) {
        DPRINTF("%s window %s mmap failed\n", __func__, win->shm_name);
        close(win->fd);
        return ret;
    }

    memory_region_init_ram_ptr(&win->wmr, OBJECT(vpeer), win->shm_name,
                               win->win_size, win->va);
    return 0;
}

static inline void read_queue_config(VirtIOPeer *vpeer)
{
    int ro, rw;

    vpeer->dev_cfg.queue_magic = VIRTIO_PEER_MAGIC;

    if(vpeer->role == MASTER) {
        ro = vpeer->dev_cfg.queue_window_idr = 0;
        rw = vpeer->dev_cfg.queue_window_idw = 1;
    } else {
        ro = vpeer->dev_cfg.queue_window_idr = 1;
        rw = vpeer->dev_cfg.queue_window_idw = 0;
    }

    vpeer->dev_cfg.windows[ro].win_phy_start = (uint64_t) vpeer->win_cfg[ro].va;
    vpeer->dev_cfg.windows[rw].win_phy_start = (uint64_t) vpeer->win_cfg[rw].va;

    vpeer->dev_cfg.windows[rw].win_size = vpeer->win_cfg[ro].win_size;
    vpeer->dev_cfg.windows[ro].win_size = vpeer->win_cfg[rw].win_size;
}

static void virtio_peer_reset(VirtIODevice *vdev)
{
    VirtIOPeer *vpeer = VIRTIO_PEER(vdev);

    DPRINTF("%s\n", __func__);
    memset(&vpeer->dev_cfg, 0, vpeer->dev_cfg_size);
    read_queue_config(vpeer);
}

static void read_args(VirtIOPeer *vpeer)
{
    vpeer->win_cfg[0].win_size = 4 << 20; // 4 MB FIXME: use cmd_size
    vpeer->win_cfg[1].win_size = 4 << 20; // 4 MB FIXME: use cmd_size
    vpeer->role = (strncmp(vpeer->args.cmd_role, "peer", 4) ? MASTER : PEER);
}

static void virtio_peer_device_realize(DeviceState *dev, Error **errp)
{
    VirtIODevice *vdev = VIRTIO_DEVICE(dev);
    VirtIOPeer *vpeer = VIRTIO_PEER(dev);

    DPRINTF("%s\n", __func__);

    read_args(vpeer);

    config_window(vpeer, 0);

    config_window(vpeer, 1);

    read_queue_config(vpeer);

    virtio_init(vdev, "virtio-peer", VIRTIO_ID_PEER, vpeer->dev_cfg_size);

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
    DEFINE_PROP_STRING("window_size", VirtIOPeer, args.cmd_size),
    DEFINE_PROP_STRING("role", VirtIOPeer, args.cmd_role),
    DEFINE_PROP_END_OF_LIST(),
};

static void virtio_peer_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    VirtioDeviceClass *vdc = VIRTIO_DEVICE_CLASS(klass);

    dc->props = virtio_peer_properties;
    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
    vdc->realize = virtio_peer_device_realize;
    vdc->unrealize = virtio_peer_device_unrealize;
    vdc->get_features = virtio_peer_get_features;
    vdc->set_features = virtio_peer_set_features;
    vdc->get_config = virtio_peer_get_config;
    vdc->set_config = virtio_peer_set_config;
    vdc->reset = virtio_peer_reset;
}

static void virtio_peer_initfn(Object *obj)
{
    VirtIOPeer *vpeer = VIRTIO_PEER(obj);

    vpeer->dev_cfg_size = sizeof(struct virtio_peer_config);
    object_property_add_link(obj, "peer", TYPE_PEER_BACKEND,
                             (Object **)&vpeer->peer,
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
    type_register_static(&virtio_peer_info);
}

type_init(virtio_register_types)
