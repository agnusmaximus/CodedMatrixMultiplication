N_ROWS=1200
N_COLS=1200
#INPUT_VECTOR=\"data/24x24uncoded/vec.dat\"
#INPUT_MATRIX=\"data/24x24uncoded/\"
#RESULT_VECTOR=\"data/24x24uncoded/result.dat\"
INPUT_VECTOR=\"data/1200x1200uncoded/vec.dat\"
INPUT_MATRIX=\"data/1200x1200uncoded/\"
RESULT_VECTOR=\"data/1200x1200uncoded/result.dat\"

DEFS=-DRESULT_VECTOR=$(RESULT_VECTOR) -DN_ROWS=$(N_ROWS) -DN_COLS=$(N_COLS) -DINPUT_VECTOR=$(INPUT_VECTOR) -DINPUT_MATRIX=$(INPUT_MATRIX)
INCLUDES=-I/usr/local/opt/openblas/include/
LIBS=-lblas

naive:
	mpic++ -DIMPLEMENTATION=0 $(LIBS) $(INCLUDES) $(DEFS) main.cpp -o naive_multiply
