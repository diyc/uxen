/*
 * Generic Balloon handlers and management
 *
 * Copyright (c) 2003-2008 Fabrice Bellard
 * Copyright (C) 2011 Red Hat, Inc.
 * Copyright (C) 2011 Amit Shah <amit.shah@redhat.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "monitor.h"
#include "cpu-common.h"
#include "kvm.h"
#include "balloon.h"
#include "trace.h"
#include "qmp-commands.h"

static QEMUBalloonEvent *balloon_event_fn;
static QEMUBalloonStatus *balloon_stat_fn;
static void *balloon_opaque;

int qemu_add_balloon_handler(QEMUBalloonEvent *event_func,
                             QEMUBalloonStatus *stat_func, void *opaque)
{
    if (balloon_event_fn || balloon_stat_fn || balloon_opaque) {
        /* We're already registered one balloon handler.  How many can
         * a guest really have?
         */
        error_report("Another balloon device already registered");
        return -1;
    }
    balloon_event_fn = event_func;
    balloon_stat_fn = stat_func;
    balloon_opaque = opaque;
    return 0;
}

void qemu_remove_balloon_handler(void *opaque)
{
    if (balloon_opaque != opaque) {
        return;
    }
    balloon_event_fn = NULL;
    balloon_stat_fn = NULL;
    balloon_opaque = NULL;
}

static int qemu_balloon(ram_addr_t target)
{
    if (!balloon_event_fn) {
        return 0;
    }
    trace_balloon_event(balloon_opaque, target);
    balloon_event_fn(balloon_opaque, target);
    return 1;
}

static int qemu_balloon_status(BalloonInfo *info)
{
    if (!balloon_stat_fn) {
        return 0;
    }
    balloon_stat_fn(balloon_opaque, info);
    return 1;
}

BalloonInfo *qmp_query_balloon(Error **errp)
{
    BalloonInfo *info;

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        error_set(errp, QERR_KVM_MISSING_CAP, "synchronous MMU", "balloon");
        return NULL;
    }

    info = g_malloc0(sizeof(*info));

    if (qemu_balloon_status(info) == 0) {
        error_set(errp, QERR_DEVICE_NOT_ACTIVE, "balloon");
        qapi_free_BalloonInfo(info);
        return NULL;
    }

    return info;
}

/**
 * do_balloon(): Request VM to change its memory allocation
 */
int do_balloon(Monitor *mon, const QDict *params,
	       MonitorCompletion cb, void *opaque)
{
    int64_t target;
    int ret;

    if (kvm_enabled() && !kvm_has_sync_mmu()) {
        qerror_report(QERR_KVM_MISSING_CAP, "synchronous MMU", "balloon");
        return -1;
    }

    target = qdict_get_int(params, "value");
    if (target <= 0) {
        qerror_report(QERR_INVALID_PARAMETER_VALUE, "target", "a size");
        return -1;
    }
    ret = qemu_balloon(target);
    if (ret == 0) {
        qerror_report(QERR_DEVICE_NOT_ACTIVE, "balloon");
        return -1;
    }

    cb(opaque, NULL);
    return 0;
}
