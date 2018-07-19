// Standard VGA mode information.
//
// Copyright (C) 2009  Kevin O'Connor <kevin@koconnor.net>
// Copyright (C) 2001-2008 the LGPL VGABios developers Team
//
// This file may be distributed under the terms of the GNU LGPLv3 license.

#include "vgabios.h" // struct VideoParamTableEntry_s
#include "biosvar.h" // GET_GLOBAL
#include "util.h" // memcpy_far
#include "stdvga.h" // stdvga_find_mode


/****************************************************************
 * Video mode register definitions
 ****************************************************************/

/* Mono */
static u8 palette0[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a, 0x2a,0x2a,0x2a,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f,
  0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f, 0x3f,0x3f,0x3f
};

static u8 palette1[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f
};

static u8 palette2[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x2a,0x00, 0x2a,0x2a,0x2a,
  0x00,0x00,0x15, 0x00,0x00,0x3f, 0x00,0x2a,0x15, 0x00,0x2a,0x3f,
  0x2a,0x00,0x15, 0x2a,0x00,0x3f, 0x2a,0x2a,0x15, 0x2a,0x2a,0x3f,
  0x00,0x15,0x00, 0x00,0x15,0x2a, 0x00,0x3f,0x00, 0x00,0x3f,0x2a,
  0x2a,0x15,0x00, 0x2a,0x15,0x2a, 0x2a,0x3f,0x00, 0x2a,0x3f,0x2a,
  0x00,0x15,0x15, 0x00,0x15,0x3f, 0x00,0x3f,0x15, 0x00,0x3f,0x3f,
  0x2a,0x15,0x15, 0x2a,0x15,0x3f, 0x2a,0x3f,0x15, 0x2a,0x3f,0x3f,
  0x15,0x00,0x00, 0x15,0x00,0x2a, 0x15,0x2a,0x00, 0x15,0x2a,0x2a,
  0x3f,0x00,0x00, 0x3f,0x00,0x2a, 0x3f,0x2a,0x00, 0x3f,0x2a,0x2a,
  0x15,0x00,0x15, 0x15,0x00,0x3f, 0x15,0x2a,0x15, 0x15,0x2a,0x3f,
  0x3f,0x00,0x15, 0x3f,0x00,0x3f, 0x3f,0x2a,0x15, 0x3f,0x2a,0x3f,
  0x15,0x15,0x00, 0x15,0x15,0x2a, 0x15,0x3f,0x00, 0x15,0x3f,0x2a,
  0x3f,0x15,0x00, 0x3f,0x15,0x2a, 0x3f,0x3f,0x00, 0x3f,0x3f,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f
};

static u8 palette3[] VAR16 = {
  0x00,0x00,0x00, 0x00,0x00,0x2a, 0x00,0x2a,0x00, 0x00,0x2a,0x2a,
  0x2a,0x00,0x00, 0x2a,0x00,0x2a, 0x2a,0x15,0x00, 0x2a,0x2a,0x2a,
  0x15,0x15,0x15, 0x15,0x15,0x3f, 0x15,0x3f,0x15, 0x15,0x3f,0x3f,
  0x3f,0x15,0x15, 0x3f,0x15,0x3f, 0x3f,0x3f,0x15, 0x3f,0x3f,0x3f,
  0x00,0x00,0x00, 0x05,0x05,0x05, 0x08,0x08,0x08, 0x0b,0x0b,0x0b,
  0x0e,0x0e,0x0e, 0x11,0x11,0x11, 0x14,0x14,0x14, 0x18,0x18,0x18,
  0x1c,0x1c,0x1c, 0x20,0x20,0x20, 0x24,0x24,0x24, 0x28,0x28,0x28,
  0x2d,0x2d,0x2d, 0x32,0x32,0x32, 0x38,0x38,0x38, 0x3f,0x3f,0x3f,
  0x00,0x00,0x3f, 0x10,0x00,0x3f, 0x1f,0x00,0x3f, 0x2f,0x00,0x3f,
  0x3f,0x00,0x3f, 0x3f,0x00,0x2f, 0x3f,0x00,0x1f, 0x3f,0x00,0x10,
  0x3f,0x00,0x00, 0x3f,0x10,0x00, 0x3f,0x1f,0x00, 0x3f,0x2f,0x00,
  0x3f,0x3f,0x00, 0x2f,0x3f,0x00, 0x1f,0x3f,0x00, 0x10,0x3f,0x00,
  0x00,0x3f,0x00, 0x00,0x3f,0x10, 0x00,0x3f,0x1f, 0x00,0x3f,0x2f,
  0x00,0x3f,0x3f, 0x00,0x2f,0x3f, 0x00,0x1f,0x3f, 0x00,0x10,0x3f,
  0x1f,0x1f,0x3f, 0x27,0x1f,0x3f, 0x2f,0x1f,0x3f, 0x37,0x1f,0x3f,
  0x3f,0x1f,0x3f, 0x3f,0x1f,0x37, 0x3f,0x1f,0x2f, 0x3f,0x1f,0x27,

  0x3f,0x1f,0x1f, 0x3f,0x27,0x1f, 0x3f,0x2f,0x1f, 0x3f,0x37,0x1f,
  0x3f,0x3f,0x1f, 0x37,0x3f,0x1f, 0x2f,0x3f,0x1f, 0x27,0x3f,0x1f,
  0x1f,0x3f,0x1f, 0x1f,0x3f,0x27, 0x1f,0x3f,0x2f, 0x1f,0x3f,0x37,
  0x1f,0x3f,0x3f, 0x1f,0x37,0x3f, 0x1f,0x2f,0x3f, 0x1f,0x27,0x3f,
  0x2d,0x2d,0x3f, 0x31,0x2d,0x3f, 0x36,0x2d,0x3f, 0x3a,0x2d,0x3f,
  0x3f,0x2d,0x3f, 0x3f,0x2d,0x3a, 0x3f,0x2d,0x36, 0x3f,0x2d,0x31,
  0x3f,0x2d,0x2d, 0x3f,0x31,0x2d, 0x3f,0x36,0x2d, 0x3f,0x3a,0x2d,
  0x3f,0x3f,0x2d, 0x3a,0x3f,0x2d, 0x36,0x3f,0x2d, 0x31,0x3f,0x2d,
  0x2d,0x3f,0x2d, 0x2d,0x3f,0x31, 0x2d,0x3f,0x36, 0x2d,0x3f,0x3a,
  0x2d,0x3f,0x3f, 0x2d,0x3a,0x3f, 0x2d,0x36,0x3f, 0x2d,0x31,0x3f,
  0x00,0x00,0x1c, 0x07,0x00,0x1c, 0x0e,0x00,0x1c, 0x15,0x00,0x1c,
  0x1c,0x00,0x1c, 0x1c,0x00,0x15, 0x1c,0x00,0x0e, 0x1c,0x00,0x07,
  0x1c,0x00,0x00, 0x1c,0x07,0x00, 0x1c,0x0e,0x00, 0x1c,0x15,0x00,
  0x1c,0x1c,0x00, 0x15,0x1c,0x00, 0x0e,0x1c,0x00, 0x07,0x1c,0x00,
  0x00,0x1c,0x00, 0x00,0x1c,0x07, 0x00,0x1c,0x0e, 0x00,0x1c,0x15,
  0x00,0x1c,0x1c, 0x00,0x15,0x1c, 0x00,0x0e,0x1c, 0x00,0x07,0x1c,

  0x0e,0x0e,0x1c, 0x11,0x0e,0x1c, 0x15,0x0e,0x1c, 0x18,0x0e,0x1c,
  0x1c,0x0e,0x1c, 0x1c,0x0e,0x18, 0x1c,0x0e,0x15, 0x1c,0x0e,0x11,
  0x1c,0x0e,0x0e, 0x1c,0x11,0x0e, 0x1c,0x15,0x0e, 0x1c,0x18,0x0e,
  0x1c,0x1c,0x0e, 0x18,0x1c,0x0e, 0x15,0x1c,0x0e, 0x11,0x1c,0x0e,
  0x0e,0x1c,0x0e, 0x0e,0x1c,0x11, 0x0e,0x1c,0x15, 0x0e,0x1c,0x18,
  0x0e,0x1c,0x1c, 0x0e,0x18,0x1c, 0x0e,0x15,0x1c, 0x0e,0x11,0x1c,
  0x14,0x14,0x1c, 0x16,0x14,0x1c, 0x18,0x14,0x1c, 0x1a,0x14,0x1c,
  0x1c,0x14,0x1c, 0x1c,0x14,0x1a, 0x1c,0x14,0x18, 0x1c,0x14,0x16,
  0x1c,0x14,0x14, 0x1c,0x16,0x14, 0x1c,0x18,0x14, 0x1c,0x1a,0x14,
  0x1c,0x1c,0x14, 0x1a,0x1c,0x14, 0x18,0x1c,0x14, 0x16,0x1c,0x14,
  0x14,0x1c,0x14, 0x14,0x1c,0x16, 0x14,0x1c,0x18, 0x14,0x1c,0x1a,
  0x14,0x1c,0x1c, 0x14,0x1a,0x1c, 0x14,0x18,0x1c, 0x14,0x16,0x1c,
  0x00,0x00,0x10, 0x04,0x00,0x10, 0x08,0x00,0x10, 0x0c,0x00,0x10,
  0x10,0x00,0x10, 0x10,0x00,0x0c, 0x10,0x00,0x08, 0x10,0x00,0x04,
  0x10,0x00,0x00, 0x10,0x04,0x00, 0x10,0x08,0x00, 0x10,0x0c,0x00,
  0x10,0x10,0x00, 0x0c,0x10,0x00, 0x08,0x10,0x00, 0x04,0x10,0x00,

  0x00,0x10,0x00, 0x00,0x10,0x04, 0x00,0x10,0x08, 0x00,0x10,0x0c,
  0x00,0x10,0x10, 0x00,0x0c,0x10, 0x00,0x08,0x10, 0x00,0x04,0x10,
  0x08,0x08,0x10, 0x0a,0x08,0x10, 0x0c,0x08,0x10, 0x0e,0x08,0x10,
  0x10,0x08,0x10, 0x10,0x08,0x0e, 0x10,0x08,0x0c, 0x10,0x08,0x0a,
  0x10,0x08,0x08, 0x10,0x0a,0x08, 0x10,0x0c,0x08, 0x10,0x0e,0x08,
  0x10,0x10,0x08, 0x0e,0x10,0x08, 0x0c,0x10,0x08, 0x0a,0x10,0x08,
  0x08,0x10,0x08, 0x08,0x10,0x0a, 0x08,0x10,0x0c, 0x08,0x10,0x0e,
  0x08,0x10,0x10, 0x08,0x0e,0x10, 0x08,0x0c,0x10, 0x08,0x0a,0x10,
  0x0b,0x0b,0x10, 0x0c,0x0b,0x10, 0x0d,0x0b,0x10, 0x0f,0x0b,0x10,
  0x10,0x0b,0x10, 0x10,0x0b,0x0f, 0x10,0x0b,0x0d, 0x10,0x0b,0x0c,
  0x10,0x0b,0x0b, 0x10,0x0c,0x0b, 0x10,0x0d,0x0b, 0x10,0x0f,0x0b,
  0x10,0x10,0x0b, 0x0f,0x10,0x0b, 0x0d,0x10,0x0b, 0x0c,0x10,0x0b,
  0x0b,0x10,0x0b, 0x0b,0x10,0x0c, 0x0b,0x10,0x0d, 0x0b,0x10,0x0f,
  0x0b,0x10,0x10, 0x0b,0x0f,0x10, 0x0b,0x0d,0x10, 0x0b,0x0c,0x10,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00,
  0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00, 0x00,0x00,0x00
};

static u8 sequ_01[] VAR16 = { 0x08, 0x03, 0x00, 0x02 };
static u8 crtc_01[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0xa0, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x1f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_01[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x0c, 0x00, 0x0f, 0x08 };
static u8 grdc_01[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x0f, 0xff };
static u8 sequ_03[] VAR16 = { 0x00, 0x03, 0x00, 0x02 };
static u8 crtc_03[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 sequ_04[] VAR16 = { 0x09, 0x03, 0x00, 0x02 };
static u8 crtc_04[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0x80, 0xbf, 0x1f,
    0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x00, 0x96, 0xb9, 0xa2,
    0xff };
static u8 actl_04[] VAR16 = {
    0x00, 0x13, 0x15, 0x17, 0x02, 0x04, 0x06, 0x07,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x01, 0x00, 0x03, 0x00 };
static u8 grdc_04[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x0f, 0x0f, 0xff };
static u8 sequ_06[] VAR16 = { 0x01, 0x01, 0x00, 0x06 };
static u8 crtc_06[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0xc1, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x00, 0x96, 0xb9, 0xc2,
    0xff };
static u8 actl_06[] VAR16 = {
    0x00, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,
    0x01, 0x00, 0x01, 0x00 };
static u8 grdc_06[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x0f, 0xff };
static u8 crtc_07[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f,
    0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x0f, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_07[] VAR16 = {
    0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
    0x10, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x0e, 0x00, 0x0f, 0x08 };
static u8 grdc_07[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0a, 0x0f, 0xff };
static u8 sequ_0d[] VAR16 = { 0x09, 0x0f, 0x00, 0x06 };
static u8 crtc_0d[] VAR16 = {
    0x2d, 0x27, 0x28, 0x90, 0x2b, 0x80, 0xbf, 0x1f,
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x14, 0x00, 0x96, 0xb9, 0xe3,
    0xff };
static u8 actl_0d[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x01, 0x00, 0x0f, 0x00 };
static u8 grdc_0d[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x0f, 0xff };
static u8 sequ_0e[] VAR16 = { 0x01, 0x0f, 0x00, 0x06 };
static u8 crtc_0e[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x00, 0x96, 0xb9, 0xe3,
    0xff };
static u8 crtc_0f[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x83, 0x85, 0x5d, 0x28, 0x0f, 0x63, 0xba, 0xe3,
    0xff };
static u8 actl_0f[] VAR16 = {
    0x00, 0x08, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00,
    0x00, 0x08, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00,
    0x01, 0x00, 0x01, 0x00 };
static u8 actl_10[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07,
    0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f,
    0x01, 0x00, 0x0f, 0x00 };
static u8 crtc_11[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x0b, 0x3e,
    0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xea, 0x8c, 0xdf, 0x28, 0x00, 0xe7, 0x04, 0xe3,
    0xff };
static u8 actl_11[] VAR16 = {
    0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f,
    0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f, 0x00, 0x3f,
    0x01, 0x00, 0x0f, 0x00 };
static u8 sequ_13[] VAR16 = { 0x01, 0x0f, 0x00, 0x0e };
static u8 crtc_13[] VAR16 = {
    0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0xbf, 0x1f,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9c, 0x8e, 0x8f, 0x28, 0x40, 0x96, 0xb9, 0xa3,
    0xff };
static u8 actl_13[] VAR16 = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
    0x41, 0x00, 0x0f, 0x00 };
static u8 grdc_13[] VAR16 = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0f, 0xff };
static u8 crtc_6A[] VAR16 = {
    0x7f, 0x63, 0x63, 0x83, 0x6b, 0x1b, 0x72, 0xf0,
    0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x59, 0x8d, 0x57, 0x32, 0x00, 0x57, 0x73, 0xe3,
    0xff };

#define PAL(x) x, sizeof(x)

struct stdvga_mode_s {
    u16 mode;
    struct vgamode_s info;

    u8 pelmask;
    u8 *dac;
    u16 dacsize;
    u8 *sequ_regs;
    u8 miscreg;
    u8 *crtc_regs;
    u8 *actl_regs;
    u8 *grdc_regs;
};

static struct stdvga_mode_s vga_modes[] VAR16 = {
    //mode { model       tx   ty bpp cw ch  sstart    }
    // pelm  dac            sequ     misc  crtc     actl     grdc
    {0x00, { MM_TEXT,    40,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_01, 0x67, crtc_01, actl_01, grdc_01},
    {0x01, { MM_TEXT,    40,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_01, 0x67, crtc_01, actl_01, grdc_01},
    {0x02, { MM_TEXT,    80,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_03, 0x67, crtc_03, actl_01, grdc_01},
    {0x03, { MM_TEXT,    80,  25, 4, 9, 16, SEG_CTEXT }
     , 0xFF, PAL(palette2), sequ_03, 0x67, crtc_03, actl_01, grdc_01},
    {0x04, { MM_CGA,    320, 200, 2, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_04, 0x63, crtc_04, actl_04, grdc_04},
    {0x05, { MM_CGA,    320, 200, 2, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_04, 0x63, crtc_04, actl_04, grdc_04},
    {0x06, { MM_CGA,    640, 200, 1, 8,  8, SEG_CTEXT }
     , 0xFF, PAL(palette1), sequ_06, 0x63, crtc_06, actl_06, grdc_06},
    {0x07, { MM_TEXT,    80,  25, 4, 9, 16, SEG_MTEXT }
     , 0xFF, PAL(palette0), sequ_03, 0x66, crtc_07, actl_07, grdc_07},
    {0x0D, { MM_PLANAR, 320, 200, 4, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette1), sequ_0d, 0x63, crtc_0d, actl_0d, grdc_0d},
    {0x0E, { MM_PLANAR, 640, 200, 4, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette1), sequ_0e, 0x63, crtc_0e, actl_0d, grdc_0d},
    {0x0F, { MM_PLANAR, 640, 350, 1, 8, 14, SEG_GRAPH }
     , 0xFF, PAL(palette0), sequ_0e, 0xa3, crtc_0f, actl_0f, grdc_0d},
    {0x10, { MM_PLANAR, 640, 350, 4, 8, 14, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xa3, crtc_0f, actl_10, grdc_0d},
    {0x11, { MM_PLANAR, 640, 480, 1, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_11, actl_11, grdc_0d},
    {0x12, { MM_PLANAR, 640, 480, 4, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_11, actl_10, grdc_0d},
    {0x13, { MM_PACKED, 320, 200, 8, 8,  8, SEG_GRAPH }
     , 0xFF, PAL(palette3), sequ_13, 0x63, crtc_13, actl_13, grdc_13},
    {0x6A, { MM_PLANAR, 800, 600, 4, 8, 16, SEG_GRAPH }
     , 0xFF, PAL(palette2), sequ_0e, 0xe3, crtc_6A, actl_10, grdc_0d},
};


/****************************************************************
 * Mode functions
 ****************************************************************/

static int
is_stdvga_mode(struct vgamode_s *vmode_g)
{
    return (vmode_g >= &vga_modes[0].info
            && vmode_g <= &vga_modes[ARRAY_SIZE(vga_modes)-1].info);
}

struct vgamode_s *
stdvga_find_mode(int mode)
{
    int i;
    for (i = 0; i < ARRAY_SIZE(vga_modes); i++) {
        struct stdvga_mode_s *stdmode_g = &vga_modes[i];
        if (GET_GLOBAL(stdmode_g->mode) == mode)
            return &stdmode_g->info;
    }
    return NULL;
}

void
stdvga_list_modes(u16 seg, u16 *dest, u16 *last)
{
    SET_FARVAR(seg, *dest, 0xffff);
}

void
stdvga_build_video_param(void)
{
    static u8 parammodes[] VAR16 = {
        0, 0, 0, 0, 0x04, 0x05, 0x06, 0x07,
        0, 0, 0, 0, 0, 0x0d, 0x0e, 0,
        0, 0x0f, 0x10, 0, 0, 0, 0, 0x01,
        0x03, 0x07, 0x11, 0x12, 0x13
    };

    int i;
    for (i=0; i<ARRAY_SIZE(parammodes); i++) {
        int mode = GET_GLOBAL(parammodes[i]);
        if (! mode)
            continue;
        struct VideoParam_s *vparam_g = &video_param_table[i];
        struct vgamode_s *vmode_g = stdvga_find_mode(mode);
        if (!vmode_g)
            continue;
        int width = GET_GLOBAL(vmode_g->width);
        int height = GET_GLOBAL(vmode_g->height);
        u8 memmodel = GET_GLOBAL(vmode_g->memmodel);
        int cheight = GET_GLOBAL(vmode_g->cheight);
        if (memmodel == MM_TEXT) {
            SET_VGA(vparam_g->twidth, width);
            SET_VGA(vparam_g->theightm1, height-1);
        } else {
            int cwidth = GET_GLOBAL(vmode_g->cwidth);
            SET_VGA(vparam_g->twidth, width / cwidth);
            SET_VGA(vparam_g->theightm1, (height / cheight) - 1);
        }
        SET_VGA(vparam_g->cheight, cheight);
        SET_VGA(vparam_g->slength, calc_page_size(memmodel, width, height));
        struct stdvga_mode_s *stdmode_g = container_of(
            vmode_g, struct stdvga_mode_s, info);
        memcpy_far(get_global_seg(), vparam_g->sequ_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->sequ_regs)
                   , ARRAY_SIZE(vparam_g->sequ_regs));
        SET_VGA(vparam_g->miscreg, GET_GLOBAL(stdmode_g->miscreg));
        memcpy_far(get_global_seg(), vparam_g->crtc_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->crtc_regs)
                   , ARRAY_SIZE(vparam_g->crtc_regs));
        memcpy_far(get_global_seg(), vparam_g->actl_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->actl_regs)
                   , ARRAY_SIZE(vparam_g->actl_regs));
        memcpy_far(get_global_seg(), vparam_g->grdc_regs
                   , get_global_seg(), GET_GLOBAL(stdmode_g->grdc_regs)
                   , ARRAY_SIZE(vparam_g->grdc_regs));
    }
}

void
stdvga_override_crtc(int mode, u8 *crtc)
{
    struct vgamode_s *vmode_g = stdvga_find_mode(mode);
    if (!vmode_g)
        return;
    struct stdvga_mode_s *stdmode_g = container_of(
        vmode_g, struct stdvga_mode_s, info);
    SET_VGA(stdmode_g->crtc_regs, crtc);
}

static void
clear_screen(struct vgamode_s *vmode_g)
{
    switch (GET_GLOBAL(vmode_g->memmodel)) {
    case MM_TEXT:
        memset16_far(GET_GLOBAL(vmode_g->sstart), 0, 0x0720, 32*1024);
        break;
#ifndef CONFIG_UXEN
    /* clearing the screen slowly triggers a concurrency bug in windows */
    case MM_CGA:
        memset16_far(GET_GLOBAL(vmode_g->sstart), 0, 0x0000, 32*1024);
        break;
    default:
        // XXX - old code gets/sets/restores sequ register 2 to 0xf -
        // but it should always be 0xf anyway.
        memset16_far(GET_GLOBAL(vmode_g->sstart), 0, 0x0000, 64*1024);
#endif  /* CONFIG_UXEN */
    }
}

int
stdvga_set_mode(struct vgamode_s *vmode_g, int flags)
{
    if (! is_stdvga_mode(vmode_g)) {
        warn_internalerror();
        return -1;
    }
    struct stdvga_mode_s *stdmode_g = container_of(
        vmode_g, struct stdvga_mode_s, info);

    // if palette loading (bit 3 of modeset ctl = 0)
    if (!(flags & MF_NOPALETTE)) {    // Set the PEL mask
        stdvga_pelmask_write(GET_GLOBAL(stdmode_g->pelmask));

        // From which palette
        u8 *palette_g = GET_GLOBAL(stdmode_g->dac);
        u16 palsize = GET_GLOBAL(stdmode_g->dacsize) / 3;

        // Always 256*3 values
        stdvga_dac_write(get_global_seg(), palette_g, 0, palsize);
        int i;
        for (i = palsize; i < 0x0100; i++) {
            static u8 rgb[3] VAR16;
            stdvga_dac_write(get_global_seg(), rgb, i, 1);
        }

        if (flags & MF_GRAYSUM)
            stdvga_perform_gray_scale_summing(0x00, 0x100);
    }

    // Set Attribute Ctl
    u8 *regs = GET_GLOBAL(stdmode_g->actl_regs);
    int i;
    for (i = 0; i <= 0x13; i++)
        stdvga_attr_write(i, GET_GLOBAL(regs[i]));
    stdvga_attr_write(0x14, 0x00);

    // Set Sequencer Ctl
    stdvga_sequ_write(0x00, 0x03);
    regs = GET_GLOBAL(stdmode_g->sequ_regs);
    for (i = 1; i <= 4; i++)
        stdvga_sequ_write(i, GET_GLOBAL(regs[i - 1]));

    // Set Grafx Ctl
    regs = GET_GLOBAL(stdmode_g->grdc_regs);
    for (i = 0; i <= 8; i++)
        stdvga_grdc_write(i, GET_GLOBAL(regs[i]));

    // Set CRTC address VGA or MDA
    u8 miscreg = GET_GLOBAL(stdmode_g->miscreg);
    u16 crtc_addr = VGAREG_VGA_CRTC_ADDRESS;
    if (!(miscreg & 1))
        crtc_addr = VGAREG_MDA_CRTC_ADDRESS;

    // Disable CRTC write protection
    stdvga_crtc_write(crtc_addr, 0x11, 0x00);
    // Set CRTC regs
    regs = GET_GLOBAL(stdmode_g->crtc_regs);
    for (i = 0; i <= 0x18; i++)
        stdvga_crtc_write(crtc_addr, i, GET_GLOBAL(regs[i]));

    // Set the misc register
    stdvga_misc_write(miscreg);

    // Enable video
    stdvga_attrindex_write(0x20);

    // Clear screen
    if (!(flags & MF_NOCLEARMEM))
        clear_screen(vmode_g);

    // Write the fonts in memory
    u8 memmodel = GET_GLOBAL(vmode_g->memmodel);
    if (memmodel == MM_TEXT)
        stdvga_load_font(get_global_seg(), vgafont16, 0x100, 0, 0, 16);

    return 0;
}
