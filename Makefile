N_ROWS=1100
N_COLS=1100
#INPUT_VECTOR=\"data/1200x1200uncoded/vec.dat\"
#INPUT_MATRIX=\"data/1200x1200uncoded/\"
#RESULT_VECTOR=\"data/1200x1200uncoded/result.dat\"
#INPUT_VECTOR=\"data/1058x1058coded1/vec.dat\"
#INPUT_MATRIX=\"data/1058x1058coded1/\"
#RESULT_VECTOR=\"data/1058x1058coded1/result.dat\"
INPUT_VECTOR=\"data/1100x1100coded2/vec.dat\"
INPUT_MATRIX=\"data/1100x1100coded2/\"
RESULT_VECTOR=\"data/1100x1100coded2/result.dat\"

DEFS=-DRESULT_VECTOR=$(RESULT_VECTOR) -DN_ROWS=$(N_ROWS) -DN_COLS=$(N_COLS) -DINPUT_VECTOR=$(INPUT_VECTOR) -DINPUT_MATRIX=$(INPUT_MATRIX)
INCLUDES=-I/usr/local/opt/openblas/include/
LIBS=-lblas

naive:
	mpic++ -DIMPLEMENTATION=0 $(LIBS) $(INCLUDES) $(DEFS) main.cpp -o naive_multiply
coded1:
	mpic++ -DIMPLEMENTATION=1 $(LIBS) $(INCLUDES) $(DEFS) main.cpp -o coded1_multiply
coded2:
	mpic++ -DIMPLEMENTATION=2 $(LIBS) $(INCLUDES) $(DEFS) main.cpp -o coded2_multiply
