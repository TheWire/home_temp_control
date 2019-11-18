BUILD_DIR = build/
SOURCE_DIR = source/
BIN = HomeThermo
BIN_DIR = bin/
BLYNK_DIR = blynk-library/src/utility/
XY_DIR= XY_433/
CXX = g++
CXXFLAGS = -c -g2 -O0 \
		-std=c++11 \
		-Wall -Weffc++ \
		-Wextra -Wcast-align \
		-Wchar-subscripts  -Wcomment -Wconversion \
		-Wdisabled-optimization \
		-Wfloat-equal  -Wformat  -Wformat=2 \
		-Wformat-nonliteral -Wformat-security  \
		-Wformat-y2k \
		-Wimport  -Winit-self  -Winline \
		-Winvalid-pch   \
		-Wunsafe-loop-optimizations  -Wlong-long -Wmissing-braces \
		-Wmissing-field-initializers -Wmissing-format-attribute   \
		-Wmissing-include-dirs -Wmissing-noreturn \
		-Wpacked  -Wparentheses  -Wpointer-arith \
		-Wredundant-decls -Wreturn-type \
		-Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector \
		-Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default \
		-Wswitch-enum -Wtrigraphs  -Wuninitialized \
		-Wunknown-pragmas  -Wunreachable-code -Wunused \
		-Wunused-function  -Wunused-label  \
		-Wunused-value  -Wunused-variable \
		-Wvolatile-register-var  -Wwrite-strings


INCLUDE = -I include/
INCLUDE_BLYNK = -I blynk-library/src/ -I blynk-library/src/utility/ -I blynk-library/linux/ -DLINUX

LIB = -lrt -lpthread -lwiringPi -lrf24 -lrf24network

all: HomeThermo

HomeThermo: checkbin checkbuild main.o thermo blynk
	$(CXX) $(BUILD_DIR)main.o \
						$(BUILD_DIR)HomeThermo.o \
						$(BUILD_DIR)log.o \
						$(BUILD_DIR)config.o \
						$(BUILD_DIR)temp.o \
						$(BLYNK_DIR)BlynkDebug.o \
						$(BLYNK_DIR)BlynkHandlers.o \
						$(BLYNK_DIR)BlynkTimer.o \
						$(LIB) \
						-o $(BIN_DIR)$(BIN)

thermo: HomeThermo.o log.o config.o temp.o

blynk: BlynkDebug.o BlynkHandlers.o BlynkTimer.o

main.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(INCLUDE_BLYNK) $(SOURCE_DIR)main.cpp -o $(BUILD_DIR)main.o

HomeThermo.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(INCLUDE_BLYNK) $(SOURCE_DIR)HomeThermo.cpp -o $(BUILD_DIR)HomeThermo.o

log.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCE_DIR)log.cpp -o $(BUILD_DIR)log.o

config.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCE_DIR)config.cpp -o $(BUILD_DIR)config.o

temp.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(SOURCE_DIR)temp.cpp -o $(BUILD_DIR)temp.o

BlynkDebug.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE_BLYNK) $(BLYNK_DIR)BlynkDebug.cpp -o $(BLYNK_DIR)BlynkDebug.o

BlynkHandlers.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE_BLYNK) $(BLYNK_DIR)BlynkHandlers.cpp -o $(BLYNK_DIR)BlynkHandlers.o

BlynkTimer.o:
	$(CXX) $(CXXFLAGS) $(INCLUDE_BLYNK) $(BLYNK_DIR)BlynkTimer.cpp -o $(BLYNK_DIR)BlynkTimer.o


.PHONY: clean
clean:
	@rm -rf $(BUILD_DIR)*
	@rm -rf $(BIN_DIR)*

checkbuild:
	@mkdir -p $(BUILD_DIR)

checkbin:
	@mkdir -p $(BIN_DIR)


	



