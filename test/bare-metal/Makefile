ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif

CC			:=	${CROSS_COMPILE}gcc

CFLAGS += -march=rv64id -mabi=lp64d

test_1: test_1.c CPU-WIP-0001.ld
	${CC} -T CPU-WIP-0001.ld test_1.c -o test_1.elf ${CFLAGS}