CC=$(GCC_CROSS)
CCAS=$(GCC_CROSS)
# Our simulator :
# - is big-endian
# - does not initialize properly fp (so we omit it), see arm_core.c
# - has a non standard main (no OS so no return value), we disable the warning
AM_CFLAGS=-mbig-endian -fomit-frame-pointer -Wno-main
# Still big-endian for ARM assembly language sources
AM_CCASFLAGS=-mbig-endian
# We don't have any OS, so no stdlib as well
# The linker script maps loadable segments into memory, the overall size must match the simulator memory
# The entry point will always be main
AM_LDFLAGS=-nostdlib -T $(srcdir)/linker_script -n --entry main
# Our simulator is big-endian
AM_LDFLAGS+=-Wl,-EB

if HAVE_ARM_COMPILER
noinst_PROGRAMS=example1 example2 example3 example4 insertion_sort ldr_str_classic ldrh_strh_classic ldrh_strh_2 ldr_str_2 ldrh_strh_ex ldr_str_3

all-am: $(PROGS)
endif

example1_SOURCES=example1.s
example2_SOURCES=example2.s
example3_SOURCES=example3.s
example4_SOURCES=example4.s
ldr_str_classic_SOURCES=ldr_str_classic.s
ldrh_strh_classic_SOURCES=ldrh_strh_classic.s
ldrh_strh_2_SOURCES=ldrh_strh_2.s
ldr_str_2_SOURCES=ldr_str_2.s
ldrh_strh_ex_SOURCES=ldrh_strh_ex.s
ldr_str_3_SOURCES=ldr_str_3.s



insertion_sort_SOURCES=insertion_sort.c my_syscalls.s my_syscalls.h

EXTRA_DIST=linker_script

clean-local:
	$(RM) $(PROGS)
