/****************************************************************************
**
** Mach64 defines
**
** Created : 940721
**
** Copyright (C) 1992-2008 Trolltech ASA.  All rights reserved.
**
** This file is part of the kernel module of the Qt GUI Toolkit.
**
** This file may be used under the terms of the GNU General
** Public License versions 2.0 or 3.0 as published by the Free
** Software Foundation and appearing in the files LICENSE.GPL2
** and LICENSE.GPL3 included in the packaging of this file.
** Alternatively you may (at your option) use any later version
** of the GNU General Public License if such license has been
** publicly approved by Trolltech ASA (or its successors, if any)
** and the KDE Free Qt Foundation.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/.
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with
** the Software.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not granted
** herein.
**
**********************************************************************/

#ifndef QGFXMACH64DEFS_QWS_H
#define QGFXMACH64DEFS_QWS_H

#ifndef QT_H
#endif // QT_H

#define GEN_TEST_CNTL 0x00d0
#define FIFO_STAT 0x0310
#define BUS_CNTL 0x00a0
#define GUI_STAT 0x0338
#define MEM_VGA_WP_SEL 0x00b4
#define MEM_VGA_RP_SEL 0x00b8
#define CONTEXT_MASK 0x0320
#define DST_OFF_PITCH 0x0100
#define DST_Y_X 0x010c
#define DST_HEIGHT 0x0114
#define DST_BRES_ERR 0x0124
#define DST_BRES_INC 0x0128
#define DST_BRES_DEC 0x012c
#define SRC_OFF_PITCH 0x0180
#define SRC_Y_X (0x63*4)
#define SRC_HEIGHT1_WIDTH1 0x0198
#define SRC_Y_X_START 0x01a4
#define SRC_HEIGHT2_WIDTH2 0x01b0
#define SRC_CNTL 0x01b4
#define SRC_LINE_X_LEFT_TO_RIGHT 0x10
#define HOST_CNTL 0x0240
#define PAT_REG0 0x0280
#define PAT_REG1 0x0284
#define PAT_CNTL 0x0288
#define SC_LEFT 0x02a0
#define SC_TOP 0x02ac
#define SC_BOTTOM 0x02b0
#define SC_RIGHT 0x02a4
#define DP_BKGD_CLR 0x02c0
#define DP_FRGD_CLR 0x02c4
#define DP_WRITE_MASK 0x02c8
#define DP_MIX 0x02d4
#define FRGD_MIX_S 0x70000
#define BKGD_MIX_D 3
#define DP_SRC 0x02d8
#define FRGD_SRC_FRGD_CLR 0x100
#define CLR_CMP_CLR 0x0300
#define CLR_CMP_MASK 0x0304
#define CLR_CMP_CNTL 0x0308
#define DP_PIX_WIDTH 0x02d0
#define HOST_32BPP 0x60000
#define HOST_16BPP 0x40000
#define HOST_8BPP 0x20000
#define HOST_1BPP 0x00000
#define SCALE_32BPP 0x60000000
#define SCALE_16BPP 0x40000000
#define SCALE_8BPP 0x20000000
#define SCALE_1BPP 0x00000000
#define SRC_32BPP 0x600
#define SRC_16BPP 0x400
#define SRC_8BPP 0x200
#define SRC_1BPP 0x000
#define DST_32BPP 0x6
#define DST_16BPP 0x4
#define DST_8BPP 0x2
#define DST_1BPP 0x0
#define BYTE_ORDER_LSB_TO_MSB 0x1000000
#define BYTE_ORDER_MSB_TO_LSB 0x0000000
#define DP_CHAIN_MASK 0x02cc
#define GUI_ENGINE_ENABLE 0x100
#define BUS_HOST_ERR_ACK 0x00800000
#define BUS_FIFO_ERR_ACK 0x00200000
#define DP_FRGD_CLR 0x02c4
#define DP_SRC 0x02d8
#define BKGD_SRC_BKGD_CLR 0
#define FRGD_SRC_FRGD_CLR 0x100
#define FRGD_MIX_AVERAGE 0x170000
#define BKGD_MIX_AVERAGE 0x0000
#define MONO_SRC_ONE 0
#define DST_X 0x0104
#define DST_Y 0x0108
#define DST_HEIGHT 0x0114
#define DST_WIDTH 0x0110
#define CONFIG_CNTL 0x00dc
#define SRC_WIDTH1 (0x64*4)
#define SRC_HEIGHT1 (0x65*4)
#define SRC_WIDTH2 (0x6a*4)
#define SRC_HEIGHT2 (0x6b*4)
#define DST_CNTL 0x0130
#define DST_HEIGHT_WIDTH 0x0118
#define GUI_CNTL (0x5e*4)

#define MIX_DST 0x0003
#define MIX_SRC 0x0007
#define SC_LEFT_RIGHT (0xaa*4)
#define SC_TOP_BOTTOM (0xad*4)

#define SCALE_3D_CNTL (0x7f*4)
#define ALPHA_TEST_CNTL (0x54*4)
#define TEX_CNTL (0xdd*4)
#define SCALE_OFF (0x70*4)
#define SCALE_PITCH (0x7b*4)
#define SCALE_WIDTH (0x77*4)
#define SCALE_HEIGHT (0x78*4)
#define SCALE_X_INC (0x7c*4)
#define SCALE_Y_INC (0x7d*4)
#define GUI_TRAJ_CNTL (0xcc*4)
#define DST_BRES_LNTH (0x48*4)

#define SCALE_HACC (0xf2*4)
#define SCALE_VACC (0x7e*4)

#define CRT_HORZ_VERT_LOAD (0x51*4)
#define CRTC_VLINE_CRNT_VLINE (0x04*4)

#define SECONDARY_SCALE_HACC (0xe9*4)
#define SECONDARY_SCALE_VACC (0xf5*4)
#define SECONDARY_SCALE_OFF (0x70*4)
#define SECONDARY_SCALE_OFF_ACC (0xe1*4)
#define SECONDARY_SCALE_X_INC (0xe7*4)
#define SECONDARY_SCALE_Y_INC (0xf4*4)
#define SECONDARY_SCALE_PITCH (0xda*4)
#define HOST_BYTE_ALIGN 1

#define CUR_HORZ_VERT_POSN (0x1b*4)
#define SETUP_CNTL (0xc1*4)
#define VERTEX_1_S (0x90*4)
#define VERTEX_1_T (0x91*4)
#define VERTEX_1_W (0x92*4)
#define VERTEX_1_SPEC_ARGB (0x93*4)
#define VERTEX_1_Z (0x94*4)
#define VERTEX_1_ARGB (0xba*4)
#define VERTEX_1_X_Y (0xbd*4)
#define VERTEX_2_S (0x98*4)
#define VERTEX_2_T (0x99*4)
#define VERTEX_2_W (0x9a*4)
#define VERTEX_2_SPEC_ARGB (0x9b*4)
#define VERTEX_2_Z (0x9c*4)
#define VERTEX_2_ARGB (0xbb*4)
#define VERTEX_2_X_Y (0xbe*4)
#define VERTEX_3_S (0xa0*4)
#define VERTEX_3_T (0xa1*4)
#define VERTEX_3_W (0xa2*4)
#define VERTEX_3_SPEC_ARGB (0xa3*4)
#define VERTEX_3_Z (0xa4*4)
#define VERTEX_3_ARGB (0xbc*4)
#define VERTEX_3_X_Y (0xbf*4)
#define ONE_OVER_AREA_UC (0xc0*4)

#define Z_CNTL (0x53*4)
#define DP_BKGD_SRC_3D 0x5
#define DP_FRGD_SRC_3D 0x500
#define DP_MONO_SRC_1 0x00000

#define TEX_SIZE_PITCH (0xdc*4)
#define TEX_0_OFFSET (0x70*4)
#define TEX_1_OFFSET (0x71*4)
#define TEX_2_OFFSET (0x72*4)
#define TEX_3_OFFSET (0x73*4)
#define TEX_4_OFFSET (0x74*4)
#define TEX_5_OFFSET (0x75*4)
#define TEX_6_OFFSET (0x76*4)
#define TEX_7_OFFSET (0x77*4)
#define TEX_8_OFFSET (0x78*4)
#define TEX_9_OFFSET (0x79*4)
#define TEX_10_OFFSET (0x7a*4)

#define CUR_CLR0 0x0060
#define CUR_CLR1 0x0064
#define CUR_OFFSET 0x0068
#define CUR_HORZ_VERT_OFF 0x0070

#endif // QGFXMACH64DEFS_QWS_H
