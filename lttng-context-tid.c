/*
 * lttng-context-tid.c
 *
 * LTTng TID context.
 *
 * Copyright (C) 2009-2012 Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
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

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <lttng-events.h>
#include <wrapper/ringbuffer/frontend_types.h>
#include <wrapper/vmalloc.h>
#include <lttng-tracer.h>

struct _jaeger_data_packed {
	uint64_t jaeger_trace_id;
	uint64_t jaeger_parent_id;
	uint64_t jaeger_span_id;
	char pad;
	pid_t tid;
	char padd;
};

union jaeger_data_packed {
	struct _jaeger_data_packed data;
	char as_char[sizeof(struct _jaeger_data_packed)];
};

#define LTTNG_JAEGER_CTX_LEN (sizeof(struct jaeger_data_packed))

static
size_t tid_get_size(size_t offset)
{
	size_t size = 0;

	size += LTTNG_JAEGER_CTX_LEN;
	return size;
}

static
void tid_record(struct lttng_ctx_field *field,
		 struct lib_ring_buffer_ctx *ctx,
		 struct lttng_channel *chan)
{
	struct jaeger_data_packed store;

	store.data.tid = task_pid_nr(current);
	store.data.jaeger_trace_id = current->jaeger_trace_id;
	store.data.jaeger_parent_id = current->jaeger_parent_id;
	store.data.jaeger_span_id = current->jaeger_span_id;

	store.data.pad = 0;
	store.data.padd = 0;

	chan->ops->event_write(ctx, store.as_char, LTTNG_JAEGER_CTX_LEN);
}

static
void tid_get_value(struct lttng_ctx_field *field,
		struct lttng_probe_ctx *lttng_probe_ctx,
		union lttng_ctx_value *value)
{
	value->str = (char *)(&current->jaeger_trace_id);
}

int lttng_add_tid_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;

	field = lttng_append_context(ctx);
	if (!field)
		return -ENOMEM;
	if (lttng_find_context(*ctx, "tid")) {
		lttng_remove_context_field(ctx, field);
		return -EEXIST;
	}
	field->event_field.name = "tid";
	field->event_field.type.atype = atype_array;
	field->event_field.type.u.array.elem_type.atype = atype_integer;
	field->event_field.type.u.array.elem_type.u.basic.integer.size = sizeof(char) * CHAR_BIT;
	field->event_field.type.u.array.elem_type.u.basic.integer.alignment = lttng_alignof(char) * CHAR_BIT;
	field->event_field.type.u.array.elem_type.u.basic.integer.signedness = lttng_is_signed_type(char);
	field->event_field.type.u.array.elem_type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.array.elem_type.u.basic.integer.base = 10;
	field->event_field.type.u.array.elem_type.u.basic.integer.encoding = lttng_encode_none;
	field->event_field.type.u.array.length = LTTNG_JAEGER_CTX_LEN;

	field->get_size = tid_get_size;
	field->record = tid_record;
	field->get_value = tid_get_value;
	lttng_context_update(*ctx);
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_tid_to_ctx);
