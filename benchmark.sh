N_PROCS=25
N_ROWS=6072
N_COLS=6072
N_COMPARE_RUNS=10

# # Naive benchmarking
rm -f uncoded_output
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} generate_uncoded_data
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} naive
for i in `seq 1 ${N_COMPARE_RUNS}`; do mpirun -np ${N_PROCS} ./naive_multiply >> uncoded_output; done
cat uncoded_output
cat uncoded_output | grep "INCORRECT"
more uncoded_output | grep "TIME ELAPSED: " | grep -oEi '[0-9]+' | awk 'NR == 1 { max=$1; min=$1; sum=0 } { if ($1>max) max=$1; if ($1<min) min=$1; sum+=$1;} END {printf "Min: %d\tMax: %d\tAverage: %f\n", min, max, sum/NR}'

# Coded1 benchmarking
rm -f coded1_output
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} generate_coded1_data
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} coded1
for i in `seq 1 ${N_COMPARE_RUNS}`; do mpirun -np ${N_PROCS} ./coded1_multiply >> coded1_output; done
cat coded1_output
cat coded1_output | grep "INCORRECT"
more coded1_output | grep "TIME ELAPSED: " | grep -oEi '[0-9]+' | awk 'NR == 1 { max=$1; min=$1; sum=0 } { if ($1>max) max=$1; if ($1<min) min=$1; sum+=$1;} END {printf "Min: %d\tMax: %d\tAverage: %f\n", min, max, sum/NR}'

# Naive benchmarking
rm -f coded2_output
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} generate_coded2_data
make N_PROCS=${N_PROCS} N_ROWS=${N_ROWS} N_COLS=${N_COLS} coded2
for i in `seq 1 ${N_COMPARE_RUNS}`; do mpirun -np ${N_PROCS} ./coded2_multiply >> coded2_output; done
cat coded2_output
cat coded2_output | grep "INCORRECT"
more coded2_output | grep "TIME ELAPSED: " | grep -oEi '[0-9]+' | awk 'NR == 1 { max=$1; min=$1; sum=0 } { if ($1>max) max=$1; if ($1<min) min=$1; sum+=$1;} END {printf "Min: %d\tMax: %d\tAverage: %f\n", min, max, sum/NR}'
