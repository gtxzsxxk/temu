ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif

CC			:=	${CROSS_COMPILE}gcc
STRIP			:=	${CROSS_COMPILE}strip

CFLAGS += -march=rv64id -mabi=lp64d

test_1: test_1.c CPU-WIP-0001.ld
	${CC} -T CPU-WIP-0001.ld -S test_1.c -o test_1.S ${CFLAGS}
	${CC} -T CPU-WIP-0001.ld test_1.c -o test_1.elf ${CFLAGS}
	${STRIP} test_1.elf

test_2: test_2.c CPU-WIP-0001.ld
	${CC} -T CPU-WIP-0001.ld -S test_2.c -o test_2.S ${CFLAGS}
	${CC} -T CPU-WIP-0001.ld test_2.c -o test_2.elf ${CFLAGS}
	${STRIP} test_2.elf
