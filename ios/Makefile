TARGET = fitplaymidware.a
INC = -I../common/protocol/cpp -I./ -I../common/third_party -I../common/midware
CFLAGS += -std=c++14
OUT_DIR = ./lib/$(BUILD_PLATFORM)
OBJ_DIR = ./obj/$(BUILD_PLATFORM)
SRC_DIR = ./

all: $(TARGET)

# Enumerating of every *.cpp as *.o and using that as dependency
$(TARGET): $(OBJ_DIR)/%.o
	$(AR) rvs $(OUT_DIR)/$(TARGET) $(OBJ_DIR)/*.o 

#Compiling every *.cpp to *.o
$(OBJ_DIR)/%.o: dirmake
	$(CC) $(CFLAGS) $(INC) -flto -O3 -c $(SRC_DIR)/*.cc -o $(OBJ_DIR)/*.o

dirmake:
	@mkdir -p $(OUT_DIR)
	@mkdir -p $(OBJ_DIR)
	
clean:
	rm -f $(OBJ_DIR)/*.o $(OUT_DIR)/$(TARGET) Makefile.bak

rebuild: clean build
