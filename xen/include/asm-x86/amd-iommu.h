/*
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Author: Leo Duran <leo.duran@amd.com>
 * Author: Wei Wang <wei.wang2@amd.com> - adapted to xen
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */
#ifndef _ASM_X86_64_AMD_IOMMU_H
#define _ASM_X86_64_AMD_IOMMU_H

#include <xen/init.h>
#include <xen/types.h>
#include <xen/list.h>
#include <xen/spinlock.h>
#include <asm/hvm/svm/amd-iommu-defs.h>

#define iommu_found()           (!list_empty(&amd_iommu_head))

extern struct list_head amd_iommu_head;

struct table_struct {
    void *buffer;
    unsigned long entries;
    unsigned long alloc_size;
};

typedef struct iommu_cap {
    uint32_t header;                    /* offset 00h */
    uint32_t base_low;                  /* offset 04h */
    uint32_t base_hi;                   /* offset 08h */
    uint32_t range;                     /* offset 0Ch */
    uint32_t misc;                      /* offset 10h */
} iommu_cap_t;

struct amd_iommu {
    struct list_head list;
    spinlock_t lock; /* protect iommu */

    u16 seg;
    u16 bdf;
    u16 cap_offset;
    iommu_cap_t cap;

    u8 ht_flags;

    void *mmio_base;
    unsigned long mmio_base_phys;

    struct table_struct dev_table;
    struct table_struct cmd_buffer;
    u32 cmd_buffer_tail;
    struct table_struct event_log;
    u32 event_log_head;

    int exclusion_enable;
    int exclusion_allow_all;
    uint64_t exclusion_base;
    uint64_t exclusion_limit;

    int msi_cap;
    int maskbit;

    int enabled;
    int irq;
};

struct ivrs_mappings {
    u16 dte_requestor_id;
    u8 dte_allow_exclusion;
    u8 unity_map_enable;
    u8 write_permission;
    u8 read_permission;
    unsigned long addr_range_start;
    unsigned long addr_range_length;
    struct amd_iommu *iommu;

    /* per device interrupt remapping table */
    void *intremap_table;
    spinlock_t intremap_lock;

    /* ivhd device data settings */
    u8 device_flags;
};

extern unsigned short ivrs_bdf_entries;

struct ivrs_mappings *get_ivrs_mappings(u16 seg);
int iterate_ivrs_mappings(int (*)(u16 seg, struct ivrs_mappings *));
int iterate_ivrs_entries(int (*)(u16 seg, struct ivrs_mappings *));

#endif /* _ASM_X86_64_AMD_IOMMU_H */
