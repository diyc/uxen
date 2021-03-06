/******************************************************************************
 * xg_private.c
 *
 * Helper functions for the rest of the library.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef _WIN32
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <winbase.h>
#include <winnt.h>
#include <winioctl.h>
#undef ERROR
#endif

#include <stdlib.h>
#include <unistd.h>
#ifndef _WIN32
#include <zlib.h>
#endif

#include "xg_private.h"


#ifdef _WIN32
static inline wchar_t *_utf8_to_wide(const char *s)
{
    int sz;
    wchar_t *ws;

    /* First figure out buffer size needed and malloc it. */
    sz = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
    if (!sz)
        return NULL;

    ws = (wchar_t *)malloc(sizeof(wchar_t) * (sz + 1));
    if (!ws)
        return NULL;
    ws[sz] = 0;

    /* Now perform the actual conversion. */
    sz = MultiByteToWideChar(CP_UTF8, 0, s, -1, ws, sz);
    if (!sz) {
        free(ws);
        ws = NULL;
    }

    return ws;
}
#endif

char *xc_read_image(xc_interface *xch,
                    const char *filename, unsigned long *size)
{
#ifndef _WIN32
    int kernel_fd = -1;
    gzFile kernel_gfd = NULL;
    char *image = NULL, *tmp;
    unsigned int bytes;

    if ( (filename == NULL) || (size == NULL) )
        return NULL;

    if ( (kernel_fd = open(filename, O_RDONLY)) < 0 )
    {
        PERROR("Could not open kernel image");
        goto out;
    }

    if ( (kernel_gfd = gzdopen(kernel_fd, "rb")) == NULL )
    {
        PERROR("Could not allocate decompression state for state file");
        goto out;
    }

    *size = 0;

#define CHUNK 1*1024*1024
    while(1)
    {
        if ( (tmp = realloc(image, *size + CHUNK)) == NULL )
        {
            PERROR("Could not allocate memory for kernel image");
            free(image);
            image = NULL;
            goto out;
        }
        image = tmp;

        bytes = gzread(kernel_gfd, image + *size, CHUNK);
        switch (bytes)
        {
        case -1:
            PERROR("Error reading kernel image");
            free(image);
            image = NULL;
            goto out;
        case 0: /* EOF */
            goto out;
        default:
            *size += bytes;
            break;
        }
    }
#undef CHUNK

 out:
    if ( *size == 0 )
    {
        PERROR("Could not read kernel image");
        free(image);
        image = NULL;
    }
    else if ( image )
    {
        /* Shrink allocation to fit image. */
        tmp = realloc(image, *size);
        if ( tmp )
            image = tmp;
    }

    if ( kernel_gfd != NULL )
        gzclose(kernel_gfd);
    else if ( kernel_fd >= 0 )
        close(kernel_fd);
    return image;
#else
    char *image = NULL;
    HANDLE ih;
    LARGE_INTEGER s;
    ULONG bytesRead;
    wchar_t *filename_w;

    filename_w = _utf8_to_wide(filename);
    if (!filename) {
        ERROR("Failed to convert utf8 to wide (%ld)", GetLastError());
        return NULL;
    }

    ih = CreateFileW(filename_w, GENERIC_READ, FILE_SHARE_READ, NULL,
                     OPEN_EXISTING, 0, NULL);

    free(filename_w);

    if (ih == INVALID_HANDLE_VALUE) {
        ERROR("CreateFileW failed (%ld)", GetLastError());
        return NULL;
    }

    if (!GetFileSizeEx(ih, &s)) {
        ERROR("GetFileSizeEx failed (%ld)", GetLastError());
        return NULL;
    }

    image = malloc((SIZE_T)s.u.LowPart);
    if (!image) {
        PERROR("malloc failed");
        return NULL;
    }

    if (!ReadFile(ih, image, s.u.LowPart, &bytesRead, NULL)) {
        ERROR("ReadFile failed (%ld)", GetLastError());
        return NULL;
    }

    if (bytesRead != s.u.LowPart) {
        ERROR("Size mismatch, read %ld bytes, expected %ld",
              bytesRead, s.u.LowPart);
        return NULL;
    }
    CloseHandle(ih);
    *size = s.u.LowPart;
    return image;
#endif
}

#if !defined(QEMU_UXEN)
char *xc_inflate_buffer(xc_interface *xch,
                        const char *in_buf, unsigned long in_size,
                        unsigned long *out_size)
{
    int           sts;
    z_stream      zStream;
    unsigned long out_len;
    char         *out_buf;

    /* Not compressed? Then return the original buffer. */
    if ( ((unsigned char)in_buf[0] != 0x1F) ||
         ((unsigned char)in_buf[1] != 0x8B) )
    {
        if ( out_size != NULL )
            *out_size = in_size;
        return (char *)in_buf;
    }

    out_len = (unsigned char)in_buf[in_size-4] +
        (256 * ((unsigned char)in_buf[in_size-3] +
                (256 * ((unsigned char)in_buf[in_size-2] +
                        (256 * (unsigned char)in_buf[in_size-1])))));

    memset(&zStream, 0, sizeof(zStream));
    out_buf = malloc(out_len + 16);        /* Leave a little extra space */
    if ( out_buf == NULL )
    {
        ERROR("Error mallocing buffer\n");
        return NULL;
    }

    zStream.next_in = (unsigned char *)in_buf;
    zStream.avail_in = in_size;
    zStream.next_out = (unsigned char *)out_buf;
    zStream.avail_out = out_len+16;
    sts = inflateInit2(&zStream, (MAX_WBITS+32)); /* +32 means "handle gzip" */
    if ( sts != Z_OK )
    {
        ERROR("inflateInit failed, sts %d\n", sts);
        free(out_buf);
        return NULL;
    }

    /* Inflate in one pass/call */
    sts = inflate(&zStream, Z_FINISH);
    inflateEnd(&zStream);
    if ( sts != Z_STREAM_END )
    {
        ERROR("inflate failed, sts %d\n", sts);
        free(out_buf);
        return NULL;
    }

    if ( out_size != NULL )
        *out_size = out_len;

    return out_buf;
}

/*******************/

int pin_table(
    xc_interface *xch, unsigned int type, unsigned long mfn, domid_t dom)
{
    struct mmuext_op op;

    op.cmd = type;
    op.arg1.mfn = mfn;

    if ( xc_mmuext_op(xch, &op, 1, dom) < 0 )
        return 1;

    return 0;
}

/* This is shared between save and restore, and may generally be useful. */
unsigned long csum_page(void *page)
{
    int i;
    unsigned long *p = page;
    unsigned long long sum=0;

    for ( i = 0; i < (PAGE_SIZE/sizeof(unsigned long)); i++ )
        sum += p[i];

    return sum ^ (sum>>32);
}

__attribute__((weak)) 
    int xc_hvm_build(xc_interface *xch,
                     uint32_t domid,
                     int memsize,
                     const char *image_name)
{
    errno = ENOSYS;
    return -1;
}
#endif  /* QEMU_UXEN */

/*
 * Local variables:
 * mode: C
 * c-set-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
