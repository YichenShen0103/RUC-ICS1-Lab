CC=gcc
CXX=g++
CFLAGS=-Wall -O0 -g -std=c11 -fsanitize=address -fno-omit-frame-pointer -fdiagnostics-color=always
CXXFLAGS=-Wall -O0 -g -std=c++17 -fsanitize=address -fno-omit-frame-pointer -fdiagnostics-color=always
CSIM_REF_FLAGS=-Wall -O3 -std=c11

ifeq ($(wildcard csim.cpp), csim.cpp)
	CSIM_CC=$(CXX)
	CSIM_FLAGS=$(CXXFLAGS)
	CSIM_SOURCE=csim.cpp
else ifeq ($(wildcard csim.cc), csim.cc)
	CSIM_CC=$(CXX)
	CSIM_FLAGS=$(CXXFLAGS)
	CSIM_SOURCE=csim.cc
else
	CSIM_CC=$(CC)
	CSIM_FLAGS=$(CFLAGS)
	CSIM_SOURCE=csim.c
endif

case_s=4
case_E=1
case_b=5

all: csim demo main

matrix.o: matrix.cpp matrix.h common.h cachelab.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c matrix.cpp

gemm.o: gemm.cpp matrix.h common.h cachelab.h
	@mkdir -p .legality_gemm
	@cp gemm.cpp .legality_gemm
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c gemm.cpp
	@ $(CXX) $(CXXFLAGS) $(CPPFLAGS) -fsyntax-only -Itest/fake_include .legality_gemm/gemm.cpp >.legality_gemm/output.txt 2>.legality_gemm/output.txt || ( \
		sed 's|\.legality_gemm/||g' .legality_gemm/output.txt && rm -rf .legality_gemm && echo "You use something illegal in your gemm.cpp!" && exit 1)
	@rm -rf .legality_gemm

gemm_baseline.o: gemm_baseline.cpp matrix.h common.h cachelab.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c gemm_baseline.cpp

test_case.o: test_case.cpp gemm.h matrix.h common.h cachelab.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c test_case.cpp

main.o: main.cpp gemm.h matrix.h common.h test_case.h cachelab.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c main.cpp

main: main.o gemm.o matrix.o test_case.o gemm_baseline.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o main main.o gemm.o gemm_baseline.o matrix.o test_case.o

demo.o: demo.cpp gemm.h matrix.h common.h cachelab.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c demo.cpp

demo: demo.o gemm.o matrix.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -o demo demo.o gemm.o matrix.o

csim: $(CSIM_SOURCE)
	$(CSIM_CC) $(CSIM_FLAGS) $(CPPFLAGS) -o csim $(CSIM_SOURCE)

csim-ref: csim-ref.c
	$(CC) ${CSIM_REF_FLAGS} $(CPPFLAGS) -o csim-ref csim-ref.c
	strip csim-ref

case%: main
	mkdir -p gemm_traces
	./main $@ > gemm_traces/$@.trace
	@if [ "$(NO_LINUX)" = "true" ]; then \
		./csim -s $(case_s) -E $(case_E) -b $(case_b) -t gemm_traces/$@.trace; \
	else \
		./csim-ref -s $(case_s) -E $(case_E) -b $(case_b) -t gemm_traces/$@.trace; \
	fi

clean:
	rm -rf main demo *.o csim gemm_traces .csim_results .overall_results .autograder_result .last_submit_time workspaces .baseline