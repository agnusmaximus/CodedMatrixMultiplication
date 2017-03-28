using namespace std;

void coded1_matrix_vector_multiply(int n_rows, int n_cols, string input_vector_filename, string input_matrix_filename, string result_vector_filename) {

    int n_procs, proc_id, name_len;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procs);
    MPI_Get_processor_name(processor_name, &name_len);
    int n_workers = n_procs-1;
    int n_rows_per_worker = (n_rows / (n_workers-1));
    int n_elements_per_worker = n_rows_per_worker * n_cols;

    if (n_rows % (n_workers-1) != 0) {
	cout << "N_ROWS not evenly divisible by n_workers-1=" << n_workers-1 << endl;
	exit(0);
    }

    double *matrix, *vec, *out, *out_final;
    matrix = vec = out = out_final = NULL;

    //Have master load input matrix / vector, worker allocates memory
    if (proc_id == 0) {
	vec = load_vector(n_cols, input_vector_filename);
	out = (double *)malloc(sizeof(double) * n_rows);
	out_final = (double *)malloc(sizeof(double) * n_rows);
    }
    else {
	vec = (double *)malloc(sizeof(double) * n_cols);
	matrix = load_matrix(n_rows_per_worker, n_cols, input_matrix_filename + to_string_util(proc_id) + ".dat");
	out = (double *)malloc(sizeof(double) * n_rows_per_worker);
	memset(out, 0, sizeof(double) * n_rows_per_worker);
    }

    //Main routine starts here
    MPI_Barrier(MPI_COMM_WORLD);

    if (proc_id == 0) {

	double *rest_sum = (double *)calloc(n_rows_per_worker, sizeof(double));
	double *parity = (double *)calloc(n_rows_per_worker, sizeof(double));

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
	    if (i == n_procs-1) {
		MPI_Irecv(parity, n_rows_per_worker, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &recv_reqs[i-1]);
	    }
	    else {
		MPI_Irecv(out_i, n_rows_per_worker, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, &recv_reqs[i-1]);
	    }
	}
	//Wait for all workers to finish
	int n_done = 0, completed[n_workers];
	memset(completed, 0, sizeof(int) * n_workers);
	while (n_done < n_workers-1) {
	    for (int i = 0; i < n_workers; i++) {
		if (!completed[i]) {
		    int did_complete = 0;
		    MPI_Test(&recv_reqs[i], &did_complete, MPI_STATUS_IGNORE);
		    if (did_complete) {
			completed[i] = 1;
			n_done += 1;

			if (i == n_workers-1) {
			    vector_vector_sum(parity, rest_sum, rest_sum, n_rows_per_worker);
			}
			else{
			    vector_vector_subtract(rest_sum, &out[n_rows_per_worker*i], rest_sum, n_rows_per_worker);

			    // We copy this workers results from out to out_final
			    double *out_i = &out[n_rows_per_worker*i];
			    double *out_final_i = &out_final[n_rows_per_worker*i];
			    memcpy(out_final_i, out_i, sizeof(double) * n_rows_per_worker);
			}
		    }
		}
	    }
	}

	//If the last process completed, and not all processes completed,
	//then compute the result of the last process
	if (completed[n_workers-1] && n_done < n_workers) {
	    int worker_incomplete = -1;
	    get_incomplete_worker(completed, n_workers, &worker_incomplete);
	    double * out_i = &out_final[n_rows_per_worker*worker_incomplete];
	    memcpy(out_i, rest_sum, sizeof(double) * n_rows_per_worker);
	}

	long long int t2 = get_time();
	check_correct(out_final, n_rows, result_vector_filename);
	cout << "TIME ELAPSED: " << t2-t1 << endl;

	free(rest_sum);
	free(parity);
	free(out_final);
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
