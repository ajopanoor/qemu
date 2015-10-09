/*
 * QEMU Virtio Peer Device Backend
 *
 * Copyright Huawei Technologies, Corp. 2015
 *
 * Authors:
 *  Ajo Jose Panoor <ajo.jose.panoor@huawei.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#include "sysemu/peer.h"
#include "qapi/qmp/qerror.h"
#include "qom/object_interfaces.h"

#define DEBUG_PEER

#ifdef DEBUG_PEER
#define DPRINTF(fmt, ...) \
do { printf("peer: " fmt , ## __VA_ARGS__); } while (0)
#else
#define DPRINTF(fmt, ...) do { } while (0)
#endif

static bool peer_backend_prop_get_opened(Object *obj, Error **errp)
{
    PeerBackend *s = PEER_BACKEND(obj);

    DPRINTF("%s\n", __func__);
    return s->opened;
}

static void peer_backend_complete(UserCreatable *uc, Error **errp)
{
    DPRINTF("%s\n", __func__);
    object_property_set_bool(OBJECT(uc), true, "opened", errp);
}

static void peer_backend_prop_set_opened(Object *obj, bool value, Error **errp)
{
    PeerBackend *s = PEER_BACKEND(obj);
    PeerBackendClass *k = PEER_BACKEND_GET_CLASS(s);
    Error *local_err = NULL;

    DPRINTF("%s\n", __func__);
    if (value == s->opened) {
        return;
    }

    if (!value && s->opened) {
        error_setg(errp, QERR_PERMISSION_DENIED);
        return;
    }

    if (k->opened) {
        k->opened(s, &local_err);
        if (local_err) {
            error_propagate(errp, local_err);
            return;
        }
    }

    s->opened = true;
}

static void peer_backend_init(Object *obj)
{
    DPRINTF("%s\n", __func__);
    object_property_add_bool(obj, "opened",
                             peer_backend_prop_get_opened,
                             peer_backend_prop_set_opened,
                             NULL);
}

static void peer_backend_class_init(ObjectClass *oc, void *data)
{
    UserCreatableClass *ucc = USER_CREATABLE_CLASS(oc);

    DPRINTF("%s\n", __func__);
    ucc->complete = peer_backend_complete;
}

static const TypeInfo peer_backend_info = {
    .name = TYPE_PEER_BACKEND,
    .parent = TYPE_OBJECT,
    .instance_size = sizeof(PeerBackend),
    .instance_init = peer_backend_init,
    .class_size = sizeof(PeerBackendClass),
    .class_init = peer_backend_class_init,
    .abstract = true,
    .interfaces = (InterfaceInfo[]) {
        { TYPE_USER_CREATABLE },
        { }
    }
};

static void register_types(void)
{
    DPRINTF("%s\n", __func__);
    type_register_static(&peer_backend_info);
}

type_init(register_types);
