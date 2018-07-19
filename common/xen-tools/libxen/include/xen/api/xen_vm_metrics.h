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

#ifndef XEN_VM_METRICS_H
#define XEN_VM_METRICS_H

#include <xen/api/xen_common.h>
#include <xen/api/xen_int_float_map.h>
#include <xen/api/xen_int_int_map.h>
#include <xen/api/xen_int_string_set_map.h>
#include <xen/api/xen_string_set.h>
#include <xen/api/xen_string_string_map.h>
#include <xen/api/xen_vm_metrics_decl.h>


/*
 * The VM_metrics class.
 * 
 * The metrics associated with a VM.
 */


/**
 * Free the given xen_vm_metrics.  The given handle must have been
 * allocated by this library.
 */
extern void
xen_vm_metrics_free(xen_vm_metrics vm_metrics);


typedef struct xen_vm_metrics_set
{
    size_t size;
    xen_vm_metrics *contents[];
} xen_vm_metrics_set;

/**
 * Allocate a xen_vm_metrics_set of the given size.
 */
extern xen_vm_metrics_set *
xen_vm_metrics_set_alloc(size_t size);

/**
 * Free the given xen_vm_metrics_set.  The given set must have been
 * allocated by this library.
 */
extern void
xen_vm_metrics_set_free(xen_vm_metrics_set *set);


typedef struct xen_vm_metrics_record
{
    xen_vm_metrics handle;
    char *uuid;
    int64_t memory_actual;
    int64_t vcpus_number;
    xen_int_float_map *vcpus_utilisation;
    xen_int_int_map *vcpus_cpu;
    xen_string_string_map *vcpus_params;
    xen_int_string_set_map *vcpus_flags;
    struct xen_string_set *state;
    time_t start_time;
    time_t last_updated;
} xen_vm_metrics_record;

/**
 * Allocate a xen_vm_metrics_record.
 */
extern xen_vm_metrics_record *
xen_vm_metrics_record_alloc(void);

/**
 * Free the given xen_vm_metrics_record, and all referenced values. 
 * The given record must have been allocated by this library.
 */
extern void
xen_vm_metrics_record_free(xen_vm_metrics_record *record);


typedef struct xen_vm_metrics_record_opt
{
    bool is_record;
    union
    {
        xen_vm_metrics handle;
        xen_vm_metrics_record *record;
    } u;
} xen_vm_metrics_record_opt;

/**
 * Allocate a xen_vm_metrics_record_opt.
 */
extern xen_vm_metrics_record_opt *
xen_vm_metrics_record_opt_alloc(void);

/**
 * Free the given xen_vm_metrics_record_opt, and all referenced values.
 *  The given record_opt must have been allocated by this library.
 */
extern void
xen_vm_metrics_record_opt_free(xen_vm_metrics_record_opt *record_opt);


typedef struct xen_vm_metrics_record_set
{
    size_t size;
    xen_vm_metrics_record *contents[];
} xen_vm_metrics_record_set;

/**
 * Allocate a xen_vm_metrics_record_set of the given size.
 */
extern xen_vm_metrics_record_set *
xen_vm_metrics_record_set_alloc(size_t size);

/**
 * Free the given xen_vm_metrics_record_set, and all referenced values.
 *  The given set must have been allocated by this library.
 */
extern void
xen_vm_metrics_record_set_free(xen_vm_metrics_record_set *set);



typedef struct xen_vm_metrics_record_opt_set
{
    size_t size;
    xen_vm_metrics_record_opt *contents[];
} xen_vm_metrics_record_opt_set;

/**
 * Allocate a xen_vm_metrics_record_opt_set of the given size.
 */
extern xen_vm_metrics_record_opt_set *
xen_vm_metrics_record_opt_set_alloc(size_t size);

/**
 * Free the given xen_vm_metrics_record_opt_set, and all referenced
 * values.  The given set must have been allocated by this library.
 */
extern void
xen_vm_metrics_record_opt_set_free(xen_vm_metrics_record_opt_set *set);


/**
 * Get a record containing the current state of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_record(xen_session *session, xen_vm_metrics_record **result, xen_vm_metrics vm_metrics);


/**
 * Get a reference to the VM_metrics instance with the specified UUID.
 */
extern bool
xen_vm_metrics_get_by_uuid(xen_session *session, xen_vm_metrics *result, char *uuid);


/**
 * Get the uuid field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_uuid(xen_session *session, char **result, xen_vm_metrics vm_metrics);


/**
 * Get the memory/actual field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_memory_actual(xen_session *session, int64_t *result, xen_vm_metrics vm_metrics);


/**
 * Get the VCPUs/number field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_vcpus_number(xen_session *session, int64_t *result, xen_vm_metrics vm_metrics);


/**
 * Get the VCPUs/utilisation field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_vcpus_utilisation(xen_session *session, xen_int_float_map **result, xen_vm_metrics vm_metrics);


/**
 * Get the VCPUs/CPU field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_vcpus_cpu(xen_session *session, xen_int_int_map **result, xen_vm_metrics vm_metrics);


/**
 * Get the VCPUs/params field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_vcpus_params(xen_session *session, xen_string_string_map **result, xen_vm_metrics vm_metrics);


/**
 * Get the VCPUs/flags field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_vcpus_flags(xen_session *session, xen_int_string_set_map **result, xen_vm_metrics vm_metrics);


/**
 * Get the state field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_state(xen_session *session, struct xen_string_set **result, xen_vm_metrics vm_metrics);


/**
 * Get the start_time field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_start_time(xen_session *session, time_t *result, xen_vm_metrics vm_metrics);


/**
 * Get the last_updated field of the given VM_metrics.
 */
extern bool
xen_vm_metrics_get_last_updated(xen_session *session, time_t *result, xen_vm_metrics vm_metrics);


/**
 * Return a list of all the VM_metrics instances known to the system.
 */
extern bool
xen_vm_metrics_get_all(xen_session *session, struct xen_vm_metrics_set **result);


#endif
