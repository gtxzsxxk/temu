ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif

ifeq (${TEST_FILE},)
$(error TEST_FILE is undefined)
endif

CC				:=	${CROSS_COMPILE}gcc
STRIP			:=	${CROSS_COMPILE}strip

CFLAGS			+= -march=rv64id -mabi=lp64d

LD_SCRIPT	:=	CPU-WIP-0001.ld

define create_elf
	@echo "Using GNU Ld script: ${LD_SCRIPT}"
	${CC} -T ${LD_SCRIPT} -S ${TEST_FILE}.c -o ${TEST_FILE}.S ${CFLAGS}
	${CC} -T ${LD_SCRIPT} ${TEST_FILE}.c -o ${TEST_FILE}.elf ${CFLAGS}
	${STRIP} ${TEST_FILE}.elf
endef

$(TEST_FILE): $(TEST_FILE).c $(LD_SCRIPT)
	$(create_elf)
