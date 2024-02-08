CROSS_COMPILE=riscv32-buildroot-linux-gnu-

ifeq (${TEST_FILE},)
$(error TEST_FILE is undefined)
endif

CC				:=	${CROSS_COMPILE}gcc
STRIP			:=	${CROSS_COMPILE}strip
OBJDUMP			:=	${CROSS_COMPILE}objdump
OBJCOPY			:=	${CROSS_COMPILE}objcopy

CFLAGS			+= -march=rv32i -mabi=ilp32 -fno-pie -no-pie -fno-stack-protector -static
CFLAGS			+= -fno-omit-frame-pointer -fno-optimize-sibling-calls -fdata-sections
CFLAGS			+= -mno-save-restore -mstrict-align -nostdlib
CFLAGS			+= -Wl,--build-id=none -N -static-libgcc -lgcc -O0

LD_SCRIPT	:=	bare-metal.LD

OBJS += start.S
OBJS += ${TEST_FILE}.c


define create_elf
	@echo "Using GNU Ld script: ${LD_SCRIPT}"
	${CC} -T ${LD_SCRIPT} ${OBJS} -o ${TEST_FILE}.elf ${CFLAGS}
#	${STRIP} ${TEST_FILE}.elf
	${OBJDUMP} ${TEST_FILE}.elf > ${TEST_FILE}.dump -D -M no-aliases,numeric
	${OBJCOPY} -O binary ${TEST_FILE}.elf ${TEST_FILE}.bin
endef

$(TEST_FILE): $(TEST_FILE).c $(LD_SCRIPT)
	$(create_elf)