using namespace std;

void coded2_matrix_vector_multiply(int n_rows, int n_cols, string input_vector_filename, string input_matrix_filename, string result_vector_filename) {

    int n_procs, proc_id, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Get_processor_name(processor_name, &name_len);
    int n_workers = n_procs-1;
    int n_rows_per_worker = (n_rows / (n_workers-2));
    int n_elements_per_worker = n_rows_per_worker * n_cols;

    if (n_rows % (n_workers-2) != 0) {
	cout << "N_ROWS not evenly divisible by n_workers-2=" << n_workers-2 << endl;
	exit(0);
    }

    double *matrix, *vec, *out;
    matrix = vec = out = NULL;

    //Have master load input matrix / vector, worker allocates memory
    if (proc_id == 0) {
	vec = load_vector(n_cols, input_vector_filename);
	out = (double *)malloc(sizeof(double) * n_rows);
    }
    else {
	vec = (double *)malloc(sizeof(double) * n_cols);
	matrix = load_matrix(n_rows_per_worker, n_cols, input_matrix_filename + to_string(proc_id) + ".dat");
	out = (double *)malloc(sizeof(double) * n_rows_per_worker);
	memset(out, 0, sizeof(double) * n_rows_per_worker);
    }

    //Main routine starts here
    MPI_Barrier(MPI_COMM_WORLD);

    if (proc_id == 0) {

	double *rest_sum1 = (double *)calloc(n_rows_per_worker, sizeof(double));
	double *rest_sum2 = (double *)calloc(n_rows_per_worker, sizeof(double));
	double *parity1 = (double *)calloc(n_rows_per_worker, sizeof(double));
	double *parity2 = (double *)calloc(n_rows_per_worker, sizeof(double));

	//Distribute vectors
	long long int t1 = get_time();
	MPI_Request send_req;
	for (int i = 1; i < n_procs; i++) {
	    MPI_Isend(vec, n_cols, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &send_req);
	}

	//Collect sub results
	MPI_Request recv_reqs[n_workers];
	for (int i = 1; i < n_procs; i++) {
	    double * out_i = &out[n_rows_per_worker*(i-1)];
	    if (i == n_procs-2) {
		MPI_Irecv(parity1, n_rows_per_worker, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &recv_reqs[i-1]);
	    }
	    else if (i == n_procs-1) {
		MPI_Irecv(parity2, n_rows_per_worker, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &recv_reqs[i-1]);
	    }
	    else {
		MPI_Irecv(out_i, n_rows_per_worker, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &recv_reqs[i-1]);
	    }
	}
	//Wait for all workers to finish
	int n_done = 0, completed[n_workers];
	memset(completed, 0, sizeof(int) * n_workers);
	while (n_done < n_workers-2) {
	    for (int i = 0; i < n_workers; i++) {
		if (!completed[i]) {
		    int did_complete = 0;
		    MPI_Test(&recv_reqs[i], &did_complete, MPI_STATUS_IGNORE);
		    if (did_complete) {
			completed[i] = 1;
			n_done += 1;
			if (i == n_workers-2) {
			    vector_vector_sum(rest_sum1, parity1, rest_sum1, n_rows_per_worker);
			}
			else if (i == n_workers-1) {
			    vector_vector_sum(rest_sum2, parity2, rest_sum2, n_rows_per_worker);
			}
			else{
			    vector_vector_subtract(rest_sum1, &out[n_rows_per_worker*i], rest_sum1, n_rows_per_worker);
			    vector_vector_add_scalark(rest_sum2, &out[n_rows_per_worker*i], rest_sum2, -(i+1), n_rows_per_worker);
			}
		    }
		}
	    }
	}

	//If only 1 missing worker
	if (n_done == n_workers-1) {
	    int worker_incomplete = -1;
	    get_incomplete_worker(completed, n_workers, &worker_incomplete);
	    if (completed[n_workers-2]) {
		if (worker_incomplete != n_workers - 1) {
		    double * out_i = &out[n_rows_per_worker*worker_incomplete];
		    memcpy(out_i, rest_sum1, sizeof(double) * n_rows_per_worker);
		}
	    }
	}
	else if (n_done == n_workers-2) {
	    int smaller_incomplete, larger_incomplete;
	    get_both_incomplete_workers(completed, n_workers, &smaller_incomplete, &larger_incomplete);
	    if (larger_incomplete == n_workers-2) {
		double * out_i = &out[n_rows_per_worker*smaller_incomplete];
		vector_vector_scalar_multiply(rest_sum2, rest_sum2, 1/(double)(smaller_incomplete+1), n_rows_per_worker);
		memcpy(out_i, rest_sum2, sizeof(double) * n_rows_per_worker);
	    }
	    if (larger_incomplete == n_workers-1 && smaller_incomplete < n_workers-2) {
		double * out_i = &out[n_rows_per_worker*smaller_incomplete];
		memcpy(out_i, rest_sum1, sizeof(double) * n_rows_per_worker);
	    }
	    if (larger_incomplete < n_workers-2) {
		double * smaller_out = &out[n_rows_per_worker*smaller_incomplete];
		double * larger_out = &out[n_rows_per_worker*larger_incomplete];
		vector_vector_add_scalark(rest_sum2, rest_sum1, rest_sum2, -(smaller_incomplete+1), n_rows_per_worker);
		vector_vector_scalar_multiply(rest_sum2, rest_sum2, 1/(double)(larger_incomplete-smaller_incomplete), n_rows_per_worker);
		vector_vector_subtract(rest_sum1, rest_sum2, rest_sum1, n_rows_per_worker);
		memcpy(larger_out, rest_sum2, sizeof(double) * n_rows_per_worker);
		memcpy(smaller_out, rest_sum1, sizeof(double) * n_rows_per_worker);
	    }
	}
	long long int t2 = get_time();
	check_correct(out, n_rows, result_vector_filename);
	cout << "TIME ELAPSED: " << t2-t1 << endl;

	free(rest_sum1);
	free(rest_sum2);
	free(parity1);
	free(parity2);
    }
    else {
	MPI_Recv(vec, n_cols, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	matrix_vector_multiply(matrix, vec, out, n_cols, n_rows_per_worker);
	MPI_Send(out, n_rows_per_worker, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    free(out);
    free(matrix);
    free(vec);
}
