N_PROCS=25
N_ROWS=1100
N_COLS=1100
#INPUT_VECTOR=\"data/1200x1200uncoded/vec.dat\"
#INPUT_MATRIX=\"data/1200x1200uncoded/\"
#RESULT_VECTOR=\"data/1200x1200uncoded/result.dat\"
#INPUT_VECTOR=\"data/1058x1058coded1/vec.dat\"
#INPUT_MATRIX=\"data/1058x1058coded1/\"
#RESULT_VECTOR=\"data/1058x1058coded1/result.dat\"
#INPUT_VECTOR=\"data/1100x1100coded2/vec.dat\"
#INPUT_MATRIX=\"data/1100x1100coded2/\"
#RESULT_VECTOR=\"data/1100x1100coded2/result.dat\"
INPUT_VECTOR=\"data/$(N_ROWS)x$(N_COLS)/vec.dat\"
INPUT_MATRIX=\"data/$(N_ROWS)x$(N_COLS)/\"
RESULT_VECTOR=\"data/$(N_ROWS)x$(N_COLS)/result.dat\"

DEFS=-DRESULT_VECTOR=$(RESULT_VECTOR) -DN_ROWS=$(N_ROWS) -DN_COLS=$(N_COLS) -DINPUT_VECTOR=$(INPUT_VECTOR) -DINPUT_MATRIX=$(INPUT_MATRIX)
INCLUDES=-I/usr/local/opt/openblas/include/
LIBS=-lblas

generate_uncoded_data:
	cd data && python generate_data.py $(N_PROCS) 0 $(N_ROWS) $(N_COLS) $(N_ROWS)x$(N_COLS) 1 && cd ..
generate_coded1_data:
	cd data && python generate_data.py $(N_PROCS) 1 $(N_ROWS) $(N_COLS) $(N_ROWS)x$(N_COLS) 1 && cd ..
generate_coded2_data:
	cd data && python generate_data.py $(N_PROCS) 2 $(N_ROWS) $(N_COLS) $(N_ROWS)x$(N_COLS) 1 && cd ..

naive:
	mpic++ -DIMPLEMENTATION=0 $(DEFS) main.cpp $(INCLUDES) $(LIBS) -o naive_multiply
coded1:
	mpic++ -DIMPLEMENTATION=1 $(DEFS) main.cpp $(INCLUDES) $(LIBS) -o coded1_multiply
coded2:
	mpic++ -DIMPLEMENTATION=2 $(DEFS) main.cpp $(INCLUDES) $(LIBS) -o coded2_multiply
