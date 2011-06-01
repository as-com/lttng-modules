/*
 * (C) Copyright	2009-2011 -
 * 		Mathieu Desnoyers <mathieu.desnoyers@efficios.com>
 *
 * LTTng priority context.
 *
 * Dual LGPL v2.1/GPL v2 license.
 */

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include "ltt-events.h"
#include "wrapper/ringbuffer/frontend_types.h"
#include "wrapper/vmalloc.h"
#include "ltt-tracer.h"

static
size_t prio_get_size(size_t offset)
{
	size_t size = 0;

	size += lib_ring_buffer_align(offset, ltt_alignof(int));
	size += sizeof(int);
	return size;
}

static
void prio_record(struct lttng_ctx_field *field,
		struct lib_ring_buffer_ctx *ctx,
		struct ltt_channel *chan)
{
	int prio;

	prio = task_prio(current);
	lib_ring_buffer_align_ctx(ctx, ltt_alignof(prio));
	chan->ops->event_write(ctx, &prio, sizeof(prio));
}

int lttng_add_prio_to_ctx(struct lttng_ctx **ctx)
{
	struct lttng_ctx_field *field;
	int ret;

	field = lttng_append_context(ctx);
	if (!field)
		return ret;
	field->event_field.name = "prio";
	field->event_field.type.atype = atype_integer;
	field->event_field.type.u.basic.integer.size = sizeof(int) * CHAR_BIT;
	field->event_field.type.u.basic.integer.alignment = ltt_alignof(int) * CHAR_BIT;
	field->event_field.type.u.basic.integer.signedness = is_signed_type(int);
	field->event_field.type.u.basic.integer.reverse_byte_order = 0;
	field->event_field.type.u.basic.integer.base = 10;
	field->event_field.type.u.basic.integer.encoding = lttng_encode_none;
	field->get_size = prio_get_size;
	field->record = prio_record;
	wrapper_vmalloc_sync_all();
	return 0;
}
EXPORT_SYMBOL_GPL(lttng_add_prio_to_ctx);

MODULE_LICENSE("GPL and additional rights");
MODULE_AUTHOR("Mathieu Desnoyers");
MODULE_DESCRIPTION("Linux Trace Toolkit Priority Context");