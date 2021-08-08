#Chip Emulator Start
CHIP_OBJS = chip/chip.o main.o
CHIP_EXEC = emulators/emu_chip
CHIPFLAGS :=

$(CHIP_EXEC) : $(CHIP_OBJS)
	@echo "Linking" $@
	$(CC) -o $@ $^ -lSDL2

$(CHIP_OBJS) : %.o : %.c
	@echo "Compiling and assembling" $@
	$(CC) -o $@ -c $< $(CHIPFLAGS)

#Chip Emulator End
clean:
	rm $(CHIP_OBJS)
	rm $(CHIP_EXEC)
