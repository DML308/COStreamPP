#ifndef _MATRIX_H_
#define _MATRIX_H_

//#define PARALLEL
#define MATRIX_DEBUG
#define MATRIX_MAX_SIZE 2000
//矩阵类
class matrix
{
public:
	//用一个二维数组保存矩阵元素
	double(*data)[MATRIX_MAX_SIZE];
	//矩阵的行数列数
	int row, col;
	matrix() :row(0), col(0), data(NULL){}
	matrix& operator =(const matrix& b)//赋值运算符
	{
		if (this != &b)
		{
			this->row = b.row;
			this->col = b.col;
			//先为等号左边的矩阵申请空间，这样就不用对操作符左边的矩阵resize了
			this->data = new double[row][MATRIX_MAX_SIZE];
			int i, j;
			for (i = 0; i < row; i++)
			{
				for (j = 0; j < col; j++)
					data[i][j] = b.data[i][j];
			}
		}
		return *this;
	}
    matrix(const matrix& b)
	{
		this->row = b.row;
		this->col = b.col;
		this->data = new double[row][MATRIX_MAX_SIZE];
		int i, j;
		for (i = 0; i < row; i++)
		{
			for (j = 0; j < col; j++)
				data[i][j] = b.data[i][j];
		}
	}
	~matrix()
	{
		delete[] data;
		data = NULL;
	}
};
//向量类
class Vector
{
public:
	//向量的数据
	double *data;
	//向量的维度
	int dim;
	Vector():data(NULL),dim(0){}
	Vector& operator =(const Vector& b)//赋值运算符
	{
		if (this != &b)
		{
			this->dim = b.dim;
			//先为等号左边的向量申请空间，这样就不用对操作符左边的向量resize了
			this->data = new double[dim];
			int i, j;
			for (i = 0; i < dim; i++)
			{
				data[i] = b.data[i];
			}
		}
		return *this;
	}
};
/***********************************矩阵操作******************************/
//申请空间，并设置矩阵行列数
//如果是创建一个特殊矩阵，则不用resize，如创建一个单位矩阵，直接
//matrix a;matrix_eye(a,rank);即可
void matrix_resize(matrix& a, int, int);

//获取矩阵中某个位置的元素
double matrix_get_num(matrix&, int, int);

//修改矩阵中某个位置的元素的值，暂时用来初始化矩阵各个元素的值
void matrix_set_num(matrix&, int, int, double);

//打印矩阵的所有元素
void matrix_print(matrix&);

//把传入的矩阵变为一个单位矩阵
void matrix_eye(matrix&,int rank);

//把传入的矩阵变为一个全0矩阵
void matrix_zeros(matrix&,int,int);

//把传入的矩阵变为一个全1矩阵
void matrix_ones(matrix&,int,int);

//重载matrix类操作符+,矩阵加法
matrix operator+(const matrix &a, const matrix &b);

//重载matrix类操作符-,矩阵减法
matrix operator-(const matrix &a, const matrix &b);

//重载matrix类操作符*，矩阵乘法
matrix operator*(const matrix &a, const matrix &b);

//将矩阵所有元素变为它们的负数
void matrix_opposite(matrix&);

//求方阵的行列式
double matrix_det(matrix&);

//方阵的LU分解
void matrix_LU(matrix& a, matrix& l, matrix u);

//矩阵转置
matrix matrix_transpose(matrix& );

//矩阵归一化
void matrix_norm(matrix&);

//获取一个方阵的上三角矩阵
matrix matrix_get_triU(matrix&);

//矩阵逆
matrix matrix_inverse(matrix&);

//矩阵水平合并
matrix matrix_hstack(matrix&, matrix&);

//矩阵垂直合并
matrix matrix_vstack(matrix&, matrix&);

/***********************************矩阵操作******************************/


/***********************************向量操作******************************/
//设定向量的维度
void Vector_resize(Vector& a, int);

//重载Vector类操作符+,向量相加
Vector operator+(const Vector &a, const Vector &b);

//重载Vector类操作符-,向量减法
Vector operator-(const Vector &a, const Vector &b);

//获取向量中的某个维度的数
double Vector_get_num(Vector&, int);

//修改向量中的某个维度的数
void Vector_set_num(Vector&, int,double);

//获取矩阵的某个行向量
Vector matrix_get_row_vector(matrix& a, int target_row);

//获取矩阵的某个列向量
Vector matrix_get_col_vector(matrix& a, int target_col);

//向量点积
double Vector_dot(Vector& a, Vector& b);

//获取向量和
double Vector_sum(Vector& a);

//获取矩阵中某个行向量的和
double matrix_sum_of_row_vector(matrix& a, int target_row);

//获取矩阵中某个列向量的和
double matrix_sum_of_col_vector(matrix& a, int target_col);

//打印向量
void Vector_print(Vector&);
/***********************************向量操作******************************/
/*
//使用二维数组来初始化matrix,需要COSteam支持new操作符，否则当使用较大规模的矩阵时会使栈空间溢出
void init(matrix a, double matrix[][MATRIX_MAX_SIZE], int row, int col)
{
	int i, j;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
		{
			a.data[i][j] = matrix[i][j];
		}
	}
}*/
#endif
