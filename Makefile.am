ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS=-fsanitize=address
AM_CFLAGS+=-D DEBUG
AM_CFLAGS+=-D WARNING
AM_CFLAGS+=-D BIG_ENDIAN_SIMULATOR
# Uncomment if performance when running with -DDEBUG is an issue
# Warning, if uncommented, issuing calls to debug functions during options
# parsing might result in debug flag incorrectly set to 0 for some files
#AM_CFLAGS+=-D CACHE_DEBUG_FLAG

LDADD=-lpthread

if HAVE_ARM_COMPILER
SUBDIRS=. Examples
endif

bin_PROGRAMS=arm_simulator send_irq memory_test registers_test arm_data_processing_test arm_branch_other_test

COMMON=csapp.h csapp.c scanner.h scanner.l debug.h debug.c \
       gdb_protocol.h gdb_protocol.c util.h util.c trace.h trace.c \
       memory.h memory.c trace_location.h no_trace_location.h \
       registers.h registers.c \
       arm.h arm.c \
       arm_constants.h arm_constants.c \
       arm_core.h arm_core.c \
       arm_exception.h arm_exception.c \
       arm_instruction.h arm_instruction.c \
       arm_data_processing.h arm_data_processing.c \
       arm_data_processing_functions.c arm_data_processing_functions.h \
       arm_load_store.h arm_load_store.c \
       arm_branch_other.h arm_branch_other.c \
       load_store_addressing_operations.c load_store_addressing_operations.h \
       load_store_memory_operations.c  load_store_memory_operations.h


arm_simulator_SOURCES=$(COMMON) arm_simulator.c
arm_branch_other_test_SOURCES= arm_branch_other_test.c $(COMMON)
send_irq_SOURCES=send_irq.c csapp.h csapp.c arm_constants.h arm_constants.c
arm_data_processing_test_SOURCES=arm_data_processing_test.c $(COMMON)
memory_test_SOURCES=memory_test.c memory.h memory.c util.h util.c
registers_test_SOURCES=registers_test.c registers.h registers.c util.h util.c arm_constants.h arm_constants.c

EXTRA_DIST=gdb_commands make_trace.sh License
