__kernel void Dilatation(__global int* regionpointer, __global int* inputSingle, __global int* outputSingle)
{
    int _n = 100;
    int _m = 100;
    int WHITE = 1;
    int BLACK = 0;
    int input[100][100];
    int transformed[100][100];
    int regionsize = regionpointer[0];

    for(int i = 0; i < _n; i++){
        for(int j = 0; j < _n; j++){
            input[i][j] = inputSingle[i * _n + j];
            transformed[i][j] = inputSingle[i * _n + j];
        }
    }
	
	int i = get_global_id(0);
        for (int j = 0; j < _m; j++) {

            if (input[i][j] == BLACK) {

                for (int k = i - regionsize / 2; k < i + regionsize / 2; k++)
                {
                    for (int l = j - regionsize / 2; l < j + regionsize / 2; l++)
                    {
                        if (k >= 0 && l >= 0 && k + regionsize / 2 < _n && l + regionsize / 2 < _m)
                            outputSingle[k * _n + l] = BLACK;
                    }
                }

            }
        }
}

__kernel void Erosion(__global int* regionpointer, __global int* inputSingle, __global int* outputSingle)
{
    int _n = 100;
    int _m = 100;
    int WHITE = 1;
    int BLACK = 0;
    int input[100][100];
    int transformed[100][100];
    int regionsize = regionpointer[0];

    for(int i = 0; i < _n; i++){
        for(int j = 0; j < _n; j++){
            input[i][j] = inputSingle[i * _n + j];
            transformed[i][j] = inputSingle[i * _n + j];
        }
    }
	
	int i = get_global_id(0);
        for (int j = 0; j < _m; j++) {

            if (input[i][j] == WHITE) {

                for (int k = i - regionsize / 2; k < i + regionsize / 2; k++)
                {
                    for (int l = j - regionsize / 2; l < j + regionsize / 2; l++)
                    {
                        if (k >= 0 && l >= 0 && k + regionsize / 2 < _n && l + regionsize / 2 < _m)
                            outputSingle[k * _n + l] = WHITE;
                    }
                }

            }
        }
}