ifeq (${CROSS_COMPILE},)
$(error CROSS_COMPILE is undefined)
endif

ifeq (${TEST_FILE},)
$(error TEST_FILE is undefined)
endif

CC				:=	${CROSS_COMPILE}gcc
STRIP			:=	${CROSS_COMPILE}strip
OBJDUMP			:=	${CROSS_COMPILE}objdump

CFLAGS			+= -march=rv64ia -mabi=lp64 -fno-pie -no-pie -fno-stack-protector -static
CFLAGS			+= -fno-omit-frame-pointer -fno-optimize-sibling-calls -fdata-sections
CFLAGS			+= -mno-save-restore -mstrict-align -nostdlib
CFLAGS			+= -Wl,--build-id=none -N -static-libgcc -lgcc

LD_SCRIPT	:=	CPU-WIP-0001.ld

define create_elf
	@echo "Using GNU Ld script: ${LD_SCRIPT}"
	${CC} -T ${LD_SCRIPT} -S ${TEST_FILE}.c -o ${TEST_FILE}.S ${CFLAGS}
	${CC} -T ${LD_SCRIPT} ${TEST_FILE}.c -o ${TEST_FILE}.elf ${CFLAGS}
#	${STRIP} ${TEST_FILE}.elf
	${OBJDUMP} ${TEST_FILE}.elf > ${TEST_FILE}.dump -D -M no-aliases,numeric
endef

$(TEST_FILE): $(TEST_FILE).c $(LD_SCRIPT)
	$(create_elf)
