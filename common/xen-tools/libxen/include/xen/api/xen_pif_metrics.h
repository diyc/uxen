/*
 * Copyright (c) 2006-2007, XenSource Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 */

#ifndef XEN_PIF_METRICS_H
#define XEN_PIF_METRICS_H

#include <xen/api/xen_common.h>
#include <xen/api/xen_pif_metrics_decl.h>


/*
 * The PIF_metrics class.
 * 
 * The metrics associated with a physical network interface.
 */


/**
 * Free the given xen_pif_metrics.  The given handle must have been
 * allocated by this library.
 */
extern void
xen_pif_metrics_free(xen_pif_metrics pif_metrics);


typedef struct xen_pif_metrics_set
{
    size_t size;
    xen_pif_metrics *contents[];
} xen_pif_metrics_set;

/**
 * Allocate a xen_pif_metrics_set of the given size.
 */
extern xen_pif_metrics_set *
xen_pif_metrics_set_alloc(size_t size);

/**
 * Free the given xen_pif_metrics_set.  The given set must have been
 * allocated by this library.
 */
extern void
xen_pif_metrics_set_free(xen_pif_metrics_set *set);


typedef struct xen_pif_metrics_record
{
    xen_pif_metrics handle;
    char *uuid;
    double io_read_kbs;
    double io_write_kbs;
    time_t last_updated;
} xen_pif_metrics_record;

/**
 * Allocate a xen_pif_metrics_record.
 */
extern xen_pif_metrics_record *
xen_pif_metrics_record_alloc(void);

/**
 * Free the given xen_pif_metrics_record, and all referenced values. 
 * The given record must have been allocated by this library.
 */
extern void
xen_pif_metrics_record_free(xen_pif_metrics_record *record);


typedef struct xen_pif_metrics_record_opt
{
    bool is_record;
    union
    {
        xen_pif_metrics handle;
        xen_pif_metrics_record *record;
    } u;
} xen_pif_metrics_record_opt;

/**
 * Allocate a xen_pif_metrics_record_opt.
 */
extern xen_pif_metrics_record_opt *
xen_pif_metrics_record_opt_alloc(void);

/**
 * Free the given xen_pif_metrics_record_opt, and all referenced
 * values.  The given record_opt must have been allocated by this library.
 */
extern void
xen_pif_metrics_record_opt_free(xen_pif_metrics_record_opt *record_opt);


typedef struct xen_pif_metrics_record_set
{
    size_t size;
    xen_pif_metrics_record *contents[];
} xen_pif_metrics_record_set;

/**
 * Allocate a xen_pif_metrics_record_set of the given size.
 */
extern xen_pif_metrics_record_set *
xen_pif_metrics_record_set_alloc(size_t size);

/**
 * Free the given xen_pif_metrics_record_set, and all referenced
 * values.  The given set must have been allocated by this library.
 */
extern void
xen_pif_metrics_record_set_free(xen_pif_metrics_record_set *set);



typedef struct xen_pif_metrics_record_opt_set
{
    size_t size;
    xen_pif_metrics_record_opt *contents[];
} xen_pif_metrics_record_opt_set;

/**
 * Allocate a xen_pif_metrics_record_opt_set of the given size.
 */
extern xen_pif_metrics_record_opt_set *
xen_pif_metrics_record_opt_set_alloc(size_t size);

/**
 * Free the given xen_pif_metrics_record_opt_set, and all referenced
 * values.  The given set must have been allocated by this library.
 */
extern void
xen_pif_metrics_record_opt_set_free(xen_pif_metrics_record_opt_set *set);


/**
 * Get a record containing the current state of the given PIF_metrics.
 */
extern bool
xen_pif_metrics_get_record(xen_session *session, xen_pif_metrics_record **result, xen_pif_metrics pif_metrics);


/**
 * Get a reference to the PIF_metrics instance with the specified UUID.
 */
extern bool
xen_pif_metrics_get_by_uuid(xen_session *session, xen_pif_metrics *result, char *uuid);


/**
 * Get the uuid field of the given PIF_metrics.
 */
extern bool
xen_pif_metrics_get_uuid(xen_session *session, char **result, xen_pif_metrics pif_metrics);


/**
 * Get the io/read_kbs field of the given PIF_metrics.
 */
extern bool
xen_pif_metrics_get_io_read_kbs(xen_session *session, double *result, xen_pif_metrics pif_metrics);


/**
 * Get the io/write_kbs field of the given PIF_metrics.
 */
extern bool
xen_pif_metrics_get_io_write_kbs(xen_session *session, double *result, xen_pif_metrics pif_metrics);


/**
 * Get the last_updated field of the given PIF_metrics.
 */
extern bool
xen_pif_metrics_get_last_updated(xen_session *session, time_t *result, xen_pif_metrics pif_metrics);


/**
 * Return a list of all the PIF_metrics instances known to the system.
 */
extern bool
xen_pif_metrics_get_all(xen_session *session, struct xen_pif_metrics_set **result);


#endif
