DEV_DIR=..
FILAMENT_DIR=$(DEV_DIR)/filament-src/filament/
FILAMENT_LIB_DIR=$(DEV_DIR)/filament-src/filament/out/cmake-webgl-release/

FILAMENT_INCLUDE = -I$(FILAMENT_DIR)filament/include -I$(FILAMENT_DIR)filament/backend/include -I$(FILAMENT_DIR)libs/utils/include -I$(FILAMENT_DIR)libs/math/include -I$(FILAMENT_DIR)libs/filameshio/include -I$(FILAMENT_DIR)libs/filabridge/include -I$(FILAMENT_DIR)libs/image/include 
INCLUDE = -Isrc -I/usr/local/include/ $(FILAMENT_INCLUDE) -Iextern/glad/include/ 

OUTPUT=out/myengine.js

SOURCES = main.cpp $(wildcard src/*.cpp) $(wildcard extern/glad/src/*.cpp)

FLAGS = -std=c++20
CC = emcc
CFLAGS = $(FLAGS) $(INCLUDE) -g

LIB_DIRS = -L/lib/x86_64-linux-gnu/ -L/lib/ -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib/ -L$(FILAMENT_LIB_DIR)/filament -L$(FILAMENT_LIB_DIR)/filament/backend -L$(FILAMENT_LIB_DIR)/libs/filabridge -L$(FILAMENT_LIB_DIR)/libs/filaflat -L$(FILAMENT_LIB_DIR)/libs/utils -L$(FILAMENT_LIB_DIR)/libs/geometry -L$(FILAMENT_LIB_DIR)/libs/ibl -L$(FILAMENT_LIB_DIR)/libs/filameshio -L$(FILAMENT_LIB_DIR)/libs/image -L$(FILAMENT_LIB_DIR)/third_party/smol-v/tnt -L$(FILAMENT_LIB_DIR)/third_party/meshoptimizer

FILAMENT_LIBS=-lfilament -lbackend -lfilabridge -lfilaflat -lutils -lgeometry -lsmol-v -libl -lfilameshio -lmeshoptimizer -limage
LIBS = $(FILAMENT_LIBS) 

all: $(SOURCES) $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CC) $(SOURCES) $(CFLAGS) $(LIBS) $(LIB_DIRS) -o $(OUTPUT) -s FULL_ES3=1 -s USE_GLFW=3 -s USE_WEBGL2=1 -s USE_LIBPNG=1 -s USE_ZLIB=1 -s ALLOW_MEMORY_GROWTH=1
    
clean:
	rm $(OUTPUT)
