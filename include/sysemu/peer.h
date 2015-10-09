/*
 * QEMU Random Number Generator Backend
 *
 * Copyright Huawei Technologies, Corp. 2015
 *
 * Authors:
 *  Ajo Jose Panoor   <ajo.jose.panoor@huawei.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or later.
 * See the COPYING file in the top-level directory.
 */

#ifndef QEMU_PEER_H
#define QEMU_PEER_H

#include "qom/object.h"
#include "qemu-common.h"
#include "qapi/error.h"

#define TYPE_PEER_BACKEND "peer-backend"
#define PEER_BACKEND(obj) \
    OBJECT_CHECK(PeerBackend, (obj), TYPE_PEER_BACKEND)
#define PEER_BACKEND_GET_CLASS(obj) \
    OBJECT_GET_CLASS(PeerBackendClass, (obj), TYPE_PEER_BACKEND)
#define PEER_BACKEND_CLASS(klass) \
    OBJECT_CLASS_CHECK(PeerBackendClass, (klass), TYPE_PEER_BACKEND)

typedef struct PeerBackendClass PeerBackendClass;
typedef struct PeerBackend PeerBackend;

struct PeerBackendClass
{
    ObjectClass parent_class;

    void (*cancel_requests)(PeerBackend *s);

    void (*opened)(PeerBackend *s, Error **errp);
};

struct PeerBackend
{
    Object parent;

    /*< protected >*/
    bool opened;
};

#endif
