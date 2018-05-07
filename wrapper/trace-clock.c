/*
 * wrapper/trace-clock.c
 *
 * Contains LTTng trace clock mapping to LTTng trace clock or mainline monotonic
 * clock. This wrapper depends on CONFIG_HIGH_RES_TIMERS=y.
 *
 * Copyright (C) 2011-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; only
 * version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <wrapper/trace-clock.h>

#ifdef LTTNG_USE_NMI_SAFE_CLOCK
DEFINE_PER_CPU(u64, lttng_last_tsc);
EXPORT_PER_CPU_SYMBOL(lttng_last_tsc);
#endif /* #ifdef LTTNG_USE_NMI_SAFE_CLOCK */

#ifdef LTTNG_CLOCK_NMI_SAFE_BROKEN
#warning "Your kernel implements a bogus nmi-safe clock source. Falling back to the non-nmi-safe clock source, which discards events traced from NMI context. Upgrade your kernel to resolve this situation."
#endif
