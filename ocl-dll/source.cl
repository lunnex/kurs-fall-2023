__kernel void Dilatation(__global int* regionpointer, __global int* inputSingle, __global int* outputSingle)
{
    int _n = 500;
    int _m = 500;
    int WHITE = 1;
    int BLACK = 0;
    int input[500][500];
    int transformed[500][500];
    int regionsize = regionpointer[0];

    int i = get_global_id(0);
    for(int j = 0; j < _n; j++){
        input[i][j] = inputSingle[i * _n + j];
		outputSingle[i * _n + j] = WHITE;
    }
    
    for (int j = 0; j < _m; j++) 
	{
        if (input[i][j] == BLACK) 
		{
			for (int l = j - regionsize / 2; l < j + regionsize / 2; l++)
            {
                if (l >= 0 && l + regionsize / 2 < _n)
				{
					outputSingle[i * _n + l] = BLACK;
				}
            }
        }
    }
	
	int j = i;
	//int j = get_global_id(0);
    for(int i1 = 0; i1 < _n; i1++){
        input[i1][j] = outputSingle[i1 * _n + j];
		//input[i1][j] = inputSingle[i1 * _n + j];
		outputSingle[i1 * _n + j] = WHITE;
    }
	
	for (int i1 = 0; i1 < _m; i1++) 
	{
        if (input[i1][j] == BLACK) 
		{
			for (int k = i1 - regionsize / 2; k < i1 + regionsize / 2; k++)
            {
                if (k >= 0 && k + regionsize / 2 < _n)
				{
					outputSingle[k * _n + j] = BLACK;
				}
            }
        }
    }
		
}

__kernel void Erosion(__global int* regionpointer, __global int* inputSingle, __global int* outputSingle)
{
        int _n = 500;
    int _m = 500;
    int WHITE = 1;
    int BLACK = 0;
    int input[500][500];
    int transformed[500][500];
    int regionsize = regionpointer[0];

    int i = get_global_id(0);
    for(int j = 0; j < _n; j++){
        input[i][j] = inputSingle[i * _n + j];
		outputSingle[i * _n + j] = BLACK;
    }
    
    for (int j = 0; j < _m; j++) 
	{
        if (input[i][j] == WHITE) 
		{
			for (int l = j - regionsize / 2; l < j + regionsize / 2; l++)
            {
                if (l >= 0 && l + regionsize / 2 < _n)
				{
					outputSingle[i * _n + l] = WHITE;
				}
            }
        }
    }
	
	int j = i;
	//int j = get_global_id(0);
    for(int i1 = 0; i1 < _n; i1++){
        input[i1][j] = outputSingle[i1 * _n + j];
		//input[i1][j] = inputSingle[i1 * _n + j];
		outputSingle[i1 * _n + j] = BLACK;
    }
	
	for (int i1 = 0; i1 < _m; i1++) 
	{
        if (input[i1][j] == WHITE) 
		{
			for (int k = i1 - regionsize / 2; k < i1 + regionsize / 2; k++)
            {
                if (k >= 0 && k + regionsize / 2 < _n)
				{
					outputSingle[k * _n + j] = WHITE;
				}
            }
        }
    }
}
