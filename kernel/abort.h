// The Mordax Microkernel
// (c) Kristian Klomsten Skordal 2013 <kristian.skordal@gmail.com>
// Report bugs and issues on <http://github.com/skordal/mordax/issues>

#ifndef MORDAX_ABORT_H
#define MORDAX_ABORT_H

struct thread_context;
void abort_handler(struct thread_context * process_block);

#endif

