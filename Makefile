# ===========
# varialbes
CC = gcc
CFLAGS = -Iinclude -Wall -pthread
SRC_DIR = src
BUILD_DIR = build
TARGET_EXE=program

# =========
# object files
OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/quick_sort.o $(BUILD_DIR)/psrs_main.o $(BUILD_DIR)/psrs_phases.o $(BUILD_DIR)/psrs_utils.o
# ===========

# benchmark target (for running benchark code only with requried compoiler flags. THIS DOES NOT USE MAIN.C OR QUICKOSRT.C as they werer for testing my own psrs implementiaons myself)
benchmark:
	mkdir -p $(BUILD_DIR)
	$(CC) -Iinclude -Wall -pthread -O2 -c $(SRC_DIR)/benchmark.c -o $(BUILD_DIR)/benchmark_opt.o
	$(CC) -Iinclude -Wall -pthread -O2 -c $(SRC_DIR)/psrs_main.c -o $(BUILD_DIR)/psrs_main_opt.o
	$(CC) -Iinclude -Wall -pthread -O2 -c $(SRC_DIR)/psrs_phases.c -o $(BUILD_DIR)/psrs_phases_opt.o
	$(CC) -Iinclude -Wall -pthread -O2 -c $(SRC_DIR)/psrs_utils.c -o $(BUILD_DIR)/psrs_utils_opt.o
	$(CC) $(BUILD_DIR)/benchmark_opt.o $(BUILD_DIR)/psrs_main_opt.o $(BUILD_DIR)/psrs_phases_opt.o $(BUILD_DIR)/psrs_utils_opt.o -pthread -o benchmark
# ===========
# if i type "make" all below before the new rules will be executed (program will be built... THAT WILL TEST MAIN, NOT THE BENCHMARK)
all: $(TARGET_EXE)

# build the program and link object files
$(TARGET_EXE): $(OBJS)
	$(CC) $(OBJS) -pthread -o $(TARGET_EXE)

# compile main.o
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o

# compile quick_sort.o
${BUILD_DIR}/quick_sort.o: ${SRC_DIR}/quick_sort.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/quick_sort.c -o $(BUILD_DIR)/quick_sort.o

# compile psrs_main.o
${BUILD_DIR}/psrs_main.o: ${SRC_DIR}/psrs_main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/psrs_main.c -o $(BUILD_DIR)/psrs_main.o

# compile psrs_phases.o
${BUILD_DIR}/psrs_phases.o: ${SRC_DIR}/psrs_phases.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/psrs_phases.c -o $(BUILD_DIR)/psrs_phases.o

# compile psrs_utils.o
${BUILD_DIR}/psrs_utils.o: ${SRC_DIR}/psrs_utils.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/psrs_utils.c -o $(BUILD_DIR)/psrs_utils.o
# ===========
# clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET_EXE)

# buld and run
run: $(TARGET_EXE)
	./$(TARGET_EXE)
# ===========