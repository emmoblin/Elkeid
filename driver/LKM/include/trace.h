/* SPDX-License-Identifier: GPL-3.0 */

#ifndef _TRACE_EVENT_H
#define _TRACE_EVENT_H
#include <linux/kernel.h>
#include <linux/trace_seq.h>
#include <linux/ring_buffer.h>
#include <linux/kallsyms.h>

#ifdef SMITH_TRACE_EVENTS
#include <linux/trace_events.h>
#else
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
#define SMITH_TRACE_EVENTS
#include <linux/trace_events.h>
#else
#include <linux/ftrace_event.h>
#endif
#endif

#define SZ_32K				0x00008000
#define SZ_128K				0x00020000

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 5, 0)
#define RING_BUFFER_ALL_CPUS -1
#endif

/* workaround for ringbuffer backporting: CentOS8 4.18.0-305.10.2.el8_4.x86_64 */
#ifndef SMITH_RINGBUFFER_STRUCT
#define ring_buffer trace_buffer
#endif

#ifdef SMITH_TRACE_EVENTS
static inline int __trace_seq_used(struct trace_seq *s)
{
	return trace_seq_used(s);
}

static inline bool __trace_seq_has_overflowed(struct trace_seq *s)
{
	return trace_seq_has_overflowed(s);
}

/*
 * Several functions return TRACE_TYPE_PARTIAL_LINE if the trace_seq
 * overflowed, and TRACE_TYPE_HANDLED otherwise. This helper function
 * simplifies those functions and keeps them in sync.
 */
static inline enum print_line_t __trace_handle_return(struct trace_seq *s)
{
	return trace_handle_return(s);
}
#else
static inline int __trace_seq_used(struct trace_seq *s)
{
    return min(s->len, (unsigned int)(PAGE_SIZE - 1));
}
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 5, 0)
static inline bool __trace_seq_has_overflowed(struct trace_seq *s)
{
    return s->len > PAGE_SIZE - 1;
}
#else
static inline bool __trace_seq_has_overflowed(struct trace_seq *s)
{
    return s->full || s->len > PAGE_SIZE - 1;
}
#endif

/*
 * Several functions return TRACE_TYPE_PARTIAL_LINE if the trace_seq
 * overflowed, and TRACE_TYPE_HANDLED otherwise. This helper function
 * simplifies those functions and keeps them in sync.
 */
static inline enum print_line_t __trace_handle_return(struct trace_seq *s)
{
    return __trace_seq_has_overflowed(s) ?
           TRACE_TYPE_PARTIAL_LINE : TRACE_TYPE_HANDLED;
}
#endif

/*
 * The print entry - the most basic unit of tracing.
 */
struct print_event_entry {
	unsigned short	id;
};

struct print_event_class {
	unsigned short id;
	enum print_line_t (*format)(struct trace_seq *seq,
				    struct print_event_entry *entry);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
	struct trace_buffer *buffer;
#else
	struct ring_buffer *buffer;
#endif
};

#define RB_BUFFER_SIZE	SZ_128K
#define PRINT_EVENT_DEFINE(name, proto, args, tstruct, assign, print)

#endif /* _TRACE_EVENT_H */
