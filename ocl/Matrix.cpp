#include <iostream>

typedef struct Matrix {
    int     a;
    int   array[100][100];
} Matrix;


class MorphologyTransformation
{
public:

	const int _n = 100;
	const int _m = 100;
	int _matrix[100][100];
	Matrix _transformed;
	//int _transformed[100][100];
	const int WHITE = 1;
	const int BLACK = 0;

	MorphologyTransformation(Matrix* matrix)
	{

		MakeDeepCopy(matrix);
		//_n = _matrix.size();
		//_m = _matrix.front().size();
	}

	int MakeDeepCopy(Matrix* matrix)
	{
		for (int i = 0; i < _n; i++) {
			for (int j = 0; j < _m; j++) {
				_matrix[i][j] = matrix->array[i][j];
				_transformed.array[i][j] = matrix->array[i][j];
			}
		}
		return 0;
	}

	int Erode(int kernel)
	{
		for (int i = 0; i < _n; i++) {
			for (int j = 0; j < _m; j++) {
				if (_matrix[i][j] == BLACK) {
					Transformation(i, j, kernel / 2, WHITE);
				}
			}
		}

		return 0;
	}

	int Dilatation(int kernel)
	{
		for (int i = 0; i < _n; i++) {
			for (int j = 0; j < _m; j++) {
				if (_matrix[i][j] == WHITE) {
					Transformation(i, j, kernel / 2, BLACK);
				}
			}
		}

		return 0;
	}

	int Transformation(int x, int y, int regionSize, int changeTo)
	{
		if (_matrix[x][y] != changeTo)
		{
			for (int i = x - regionSize; i < x + regionSize; i++)
			{
				for (int j = y - regionSize; j < y + regionSize; j++)
				{
					if (i >= 0 && j >= 0 && x + regionSize < _n && y + regionSize < _m && IsTransformable(x, y, regionSize, changeTo))
						_transformed.array[i][j] = changeTo;
				}
			}
		}

		return 0;
	}

	bool IsTransformable(int x, int y, int regionSize, int changeTo)
	{
		bool result = false;

		for (int i = x - regionSize; i < x + regionSize; i++)
		{
			for (int j = y - regionSize; j < y + regionSize; j++)
			{
				if (i >= 0 && j >= 0 && x + regionSize < _n && y + regionSize < _m)
					result |= _matrix[i][j] == changeTo;
			}
		}

		return result;
	}

	int Print() {
		for (int i = 0; i < _n; i++) {
			for (int j = 0; j < _m; j++) {
				std::cout << _transformed.array[i][j];
			}
			std::cout << "\n";
		}

		return 0;
	}


};