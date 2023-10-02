// SPDX-License-Identifier: GPL-2.0-only
/*
 * Derived from eprog_user_blob.S by Richard Weinberger
 * Copyright (C) 2023 Richard Weinberger <richard@nod.at>
 * Copyright (C) 2023 Vulcan Ignis <1ohm@pm.me>
 */
        .section .init.rodata, "a"
        .global embedded_umh_start
embedded_umh_start:
        .incbin "drivers/misc/l2e_os/l2e_bin"
        .global embedded_umh_end
embedded_umh_end:
