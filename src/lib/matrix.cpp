//#include "omp.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include<iostream>
#include<time.h>
#include<iomanip>
#include<string>
#include "matrix.h"

using namespace std;
/**********************************常量定义*******************************/
//一些常量定义
#define  MATRIX_MAX_SIZE 2000
#define  MATRIX_MAX_SIZE 2000
//非三角矩阵，上三角矩阵，下三角矩阵，对角阵
enum Tri_matrix_type{ Not_tri_matrix, TriU_matirx, TriL_matirx, Diag_matrix };

/**********************************常量定义*******************************/

//申请空间，并设置矩阵行列数
void matrix_resize(matrix& a, int row, int col)
{
	a.row = row;
	a.col = col;
	a.data = new double[row][MATRIX_MAX_SIZE];
	//初始化为0
	int i, j;
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
			a.data[i][j] = 0;
	}

}

//获取矩阵中某个位置的元素
double matrix_get_num(matrix& a, int target_row, int target_col)
{
	return a.data[target_row][target_col];
}

//修改矩阵中某个位置的元素的值，暂时用来初始化矩阵各个元素的值
void matrix_set_num(matrix& a, int target_row, int target_col, double value)
{
	a.data[target_row][target_col] = value;
}

//打印矩阵的所有元素
void matrix_print(matrix& a)
{
	int i, j;
	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
		{
			cout << a.data[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

//重载matrix类操作符+,矩阵加法
matrix operator+(const matrix &a, const matrix &b)
{
	matrix c;
	int i, j;
	c.row = a.row;
	c.col = a.col;
	c.data = new double[c.row][MATRIX_MAX_SIZE];

	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
		{
			c.data[i][j] = a.data[i][j] + b.data[i][j];
		}
	}
	return c;
}

//重载matrix类操作符-,矩阵减法
matrix operator-(const matrix &a, const matrix &b)
{
	matrix c;
	int i, j;
	c.row = a.row;
	c.col = a.col;
	c.data = new double[c.row][MATRIX_MAX_SIZE];

	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
		{
			c.data[i][j] = a.data[i][j] - b.data[i][j];
		}
	}
	return c;
}

//前置声明
void mul(double matrix1[][MATRIX_MAX_SIZE], int row1, int col1, double matrix2[][MATRIX_MAX_SIZE], int row2, int col2, double matrix[][MATRIX_MAX_SIZE]);
//重载matrix类操作符*，矩阵乘法
matrix operator*(const matrix &a, const matrix &b)
{
	matrix c;
	int i, j;
	c.row = a.row;
	c.col = b.col;
	c.data = new double[c.row][MATRIX_MAX_SIZE];
	mul(a.data,a.row,a.col,b.data,b.row,b.col,c.data);
	return c;
}

//创建单位矩阵
void matrix_eye(matrix& a,int rank)
{
	int i, j;
	matrix_resize(a, rank, rank);
	for (i = 0; i < rank; i++)
	{
		for (j = 0; j < rank; j++)
			a.data[i][j] = 0;
	}
	for (i = 0; i < rank; i++)
		a.data[i][i] = 1;
}

//创建全零矩阵
void matrix_zeros(matrix& a, int row, int col)
{
	int i, j;
	matrix_resize(a, row, col);
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
			a.data[i][j] = 0;
	}
}

//创建全1矩阵
void matrix_ones(matrix& a, int row, int col)
{
	int i, j;
	matrix_resize(a, row, col);
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < col; j++)
			a.data[i][j] = 1;
	}
}

//置为相反数
void matrix_opposite(matrix& a)
{
	int i, j;
	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
			a.data[i][j] = -a.data[i][j];
	}
}

//求方阵的行列式
//前置声明
double det(double matrix[][MATRIX_MAX_SIZE], int rank);
double matrix_det(matrix& a)
{
	return det(a.data, a.row);
}

//方阵的LU分解
//前置声明
void lu(double a[][MATRIX_MAX_SIZE], int rank, double l[][MATRIX_MAX_SIZE], double u[][MATRIX_MAX_SIZE]);
void matrix_LU(matrix& a,matrix& l,matrix u)
{
	matrix a1 = a;
	lu(a1.data,a1.row,l.data,u.data);
}

//矩阵转置
matrix matrix_transpose(matrix& a)
{
	matrix a_T;
	matrix_resize(a_T, a.col, a.row);
	int i, j;
	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j<a.col; j++)
			a_T.data[j][i] = a.data[i][j];
	}
	return a_T;
}

//矩阵归一化
void matrix_norm(matrix& a)
{
	int i, j;
	double (*matrix)[MATRIX_MAX_SIZE]=a.data;
#ifdef PARALLEL
	omp_set_num_threads(4);
#pragma omp parallel for shared(matrix) private(j,i)
	for (j = 0; j<a.col; j++)
	{
		double s = 0;
		for (i = 0; i<a.row; i++)
			s += matrix[i][j] * matrix[i][j];
		s = sqrt(s);
		for (i = 0; i<a.row; i++)
			matrix[i][j] /= s;
	}
#else
	for (j = 0; j<a.col; j++)
	{
		double s = 0;
		for (i = 0; i<a.row; i++)
			s += matrix[i][j] * matrix[i][j];
		s = sqrt(s);
		for (i = 0; i<a.row; i++)
			matrix[i][j] /= s;
	}
#endif
}

//获取一个方阵的上三角矩阵
//前置声明
void get_triU(double a[][MATRIX_MAX_SIZE], int rank);
matrix matrix_get_triU(matrix& a)
{
	matrix a_U;
	matrix_resize(a_U, a.row, a.col);
	get_triU(a_U.data, a.row);
	return a_U;
}

//矩阵逆
//前置声明
void inverse(double A[][MATRIX_MAX_SIZE], int rank, double a[][MATRIX_MAX_SIZE]);
matrix matrix_inverse(matrix& A)
{
	matrix a;
	matrix_resize(a, A.row, A.row);
	matrix A1 = A;
	inverse(A1.data, A.row, a.data);
	return a;
}

//矩阵水平合并
matrix matrix_hstack(matrix& a, matrix& b)
{
	matrix c;
	matrix_resize(c, a.row, a.col + b.col);
	int i, j;
	for (i = 0; i < a.row; i++)
	{
		for (j = 0; j < a.col; j++)
			c.data[i][j] = a.data[i][j];
		for (j = 0; j < b.col; j++)
			c.data[i][j + a.col] = b.data[i][j];
	}
	return c;
}

//矩阵垂直合并
matrix matrix_vstack(matrix& a, matrix& b)
{
	matrix c;
	matrix_resize(c, a.row+b.row, a.col );
	int i, j;
	for (j = 0; j < a.col; j++)
	{
		for (i = 0; i < a.row; i++)
			c.data[i][j] = a.data[i][j];
		for (i = 0; i < b.row; i++)
			c.data[i+a.row][j] = b.data[i][j];
	}
	return c;
}
//设定向量的维度
void Vector_resize(Vector& a, int dim)
{
	a.dim = dim;
	a.data = new double[dim];
	int i;
	for (i = 0; i < dim; i++)
		a.data[i] = 0;
}
//重载Vector类操作符+,向量相加
Vector operator+(const Vector &a, const Vector &b)
{
	Vector c;
	Vector_resize(c, a.dim);
	int i;
	for (i = 0; i < a.dim; i++)
		c.data[i] = a.data[i] + b.data[i];
	return c;
}
//重载Vector类操作符-,向量相减
Vector operator-(const Vector &a, const Vector &b)
{
	Vector c;
	Vector_resize(c, a.dim);
	int i;
	for (i = 0; i < a.dim; i++)
		c.data[i] = a.data[i] - b.data[i];
	return c;
}

//获取向量中的某个维度的数
double Vector_get_num(Vector& a, int target_dim)
{
	return a.data[target_dim];
}
//修改向量中的某个维度的数
void Vector_set_num(Vector& a, int target_dim, double value)
{
	a.data[target_dim] = value;
}
//获取矩阵的某个行向量
Vector matrix_get_row_vector(matrix& a, int target_row)
{
	Vector c;
	Vector_resize(c, a.col);
	int j;
	for (j = 0; j < a.col; j++)
		c.data[j] = a.data[target_row][j];
	return c;
}
//获取矩阵的某个列向量
Vector matrix_get_col_vector(matrix& a, int target_col)
{
	Vector c;
	Vector_resize(c, a.row);
	int i;
	for (i = 0; i < a.row; i++)
		c.data[i] = a.data[i][target_col];
	return c;
}

//向量点积
double Vector_dot(Vector& a,Vector& b)
{
	int i;
	double sum = 0;
	for (i = 0; i<a.dim; i++)
		sum += a.data[i] * b.data[i];
	return sum;
}
//获取向量和
double Vector_sum(Vector& a)
{
	int i;
	double sum = 0;
	for (i = 0; i<a.dim; i++)
		sum += a.data[i];
	return sum;
}
//获取矩阵中某个行向量的和
double matrix_sum_of_row_vector(matrix& a,int target_row)
{
	Vector c = matrix_get_row_vector(a, target_row);
	int i;
	double sum;
	sum = Vector_sum(c);
	return sum;
}
//获取矩阵中某个列向量的和
double matrix_sum_of_col_vector(matrix& a, int target_col)
{
	Vector c = matrix_get_col_vector(a, target_col);
	int i;
	double sum;
	sum = Vector_sum(c);
	return sum;
}
//打印向量
void Vector_print(Vector& a)
{
	int i;
	for (i = 0; i < a.dim; i++)
		cout << a.data[i] << " ";
	cout << endl;
}
//******************Cannon并行矩阵乘算法************************//
//此方法需要矩阵规模能被4整除，否则应该要边界补零，最好使用openmpfor循环展开的mul函数
//此算法在论文的3.2.1节
			//*************START****************//
void MoveDataBlockLine_A(double A[][ MATRIX_MAX_SIZE], int lineNumber) // A 左移一块
{
	int i, j, k, jump =  MATRIX_MAX_SIZE / 4;
	int tmp[ MATRIX_MAX_SIZE / 4][ MATRIX_MAX_SIZE / 4];

	for (i = 0; i<jump; i++)
	for (j = 0; j<jump; j++)
		tmp[i][j] = A[i + jump*lineNumber][j]; // 保存块 A(i,0)

	for (k = 0; k<3; k++)
	{
		for (i = 0; i<jump; i++)
		for (j = 0; j<jump; j++)
			A[i + jump*lineNumber][j + jump*k] = A[i + jump*lineNumber][j + jump*(k + 1)];
	}

	for (i = 0; i<jump; i++)
	for (j = 0; j<jump; j++)
		A[i + jump*lineNumber][j + jump * 3] = tmp[i][j];
}

void MoveDataBlockLine_B(double B[][ MATRIX_MAX_SIZE], int lineNumber) // B 上移一块
{
	int i, j, k, jump =  MATRIX_MAX_SIZE / 4;
	int tmp[ MATRIX_MAX_SIZE / 4][ MATRIX_MAX_SIZE / 4];

	for (i = 0; i<jump; i++)
	for (j = 0; j<jump; j++)
		tmp[i][j] = B[i][j + jump*lineNumber]; // 保存块 b(0,j)

	for (k = 0; k<3; k++)
	{
		for (i = 0; i<jump; i++)
		for (j = 0; j<jump; j++)
			B[i + jump*k][j + jump*lineNumber] = B[i + jump*(k + 1)][j + jump*lineNumber];
	}

	for (i = 0; i<jump; i++)
	for (j = 0; j<jump; j++)
		B[i + jump * 3][j + jump*lineNumber] = tmp[i][j];
}

void MutiplyBlock(double A[][ MATRIX_MAX_SIZE], double B[][ MATRIX_MAX_SIZE], double C[][ MATRIX_MAX_SIZE], int bi, int bj)
{
	int i, j, jump =  MATRIX_MAX_SIZE / 4, k;
	for (i = 0; i<jump; i++)
	for (j = 0; j<jump; j++)
	for (k = 0; k<jump; k++)
	{
		C[i + jump*bi][j + jump*bj] += A[i + jump*bi][j + jump*bj + k] * B[i + jump*bi + k][j + jump*bj];
	}
}
//void cannon(double A[][ MATRIX_MAX_SIZE], double B[][ MATRIX_MAX_SIZE], double C[][ MATRIX_MAX_SIZE])
//{
//
//	int i, j, k, jump =  MATRIX_MAX_SIZE / 4;
//	//int A[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE], B[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE], C[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE];
//	//int (* A)[ MATRIX_MAX_SIZE] = new int[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE];
//	//int (* B)[ MATRIX_MAX_SIZE] = new int[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE];
//	//int (* C)[ MATRIX_MAX_SIZE] = new int[ MATRIX_MAX_SIZE][ MATRIX_MAX_SIZE];
//	//printf( "\n开始 Cannon 计算 ..." );
//	//fpA = fopen( "A.txt", "w+" );
//	//fpB = fopen( "B.txt", "w+" );
//	/* 赋初值 */
//	//int m=0;
//	for (i = 0; i< MATRIX_MAX_SIZE; i++)
//	for (j = 0; j< MATRIX_MAX_SIZE; j++)
//	{
//		//A[i][j] = m;//rand()%10;  fprintf( fpA, "\n A[%d][%d] = %d ", i, j, A[i][j] );
//		//B[i][j] = m;//rand()%10;  fprintf( fpB, "\n B[%d][%d] = %d ", i, j, B[i][j] );
//		C[i][j] = 0;
//		//m++;
//	}
//	//fclose(fpA);
//	//fclose(fpB);
//
//	// 设置 omp 线程数
//	omp_set_num_threads(4);
//
//	if ( MATRIX_MAX_SIZE % 4 == 0)
//	{
//		#pragma omp parallel shared( A, B, C ) private( i,j )
//		{
//			#pragma omp sections
//			{
//				// 第一次移动数据
//				#pragma omp section
//				{
//					//printf("\n Sections 1, thread number %d ", omp_get_thread_num() );
//					MoveDataBlockLine_A(A, 1);
//					MoveDataBlockLine_A(A, 2);
//					MoveDataBlockLine_A(A, 2);
//					MoveDataBlockLine_A(A, 3);
//					MoveDataBlockLine_A(A, 3);
//					MoveDataBlockLine_A(A, 3);
//				}
//
//				#pragma omp section
//				{
//				//printf("\n Sections 1, thread number %d ", omp_get_thread_num() );
//				MoveDataBlockLine_B(B, 1);
//				MoveDataBlockLine_B(B, 2);
//				MoveDataBlockLine_B(B, 2);
//				MoveDataBlockLine_B(B, 3);
//				MoveDataBlockLine_B(B, 3);
//				MoveDataBlockLine_B(B, 3);
//			}
//			}
//		}
//		// 开始循环计算
//		// 先进行计算,然后再移动数据
//		for (k = 0; k < 4; k++)
//		{
//			#pragma omp parallel shared( A, B, C ) private( i,j )
//			{
//				#pragma omp sections
//				{
//					#pragma omp section  // 一个线程用来移动A的上半部分 和 B的左半部分
//					{
//						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num() );
//						i = 0;
//						for (j = 0; j < 4; j++)
//							MutiplyBlock(A, B, C, i, j);
//					}
//
//					#pragma omp section // 一个线程用来移动A的下半部分 和 B的右半部分
//					{
//					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num() );
//					i = 1;
//					for (j = 0; j < 4; j++)
//						MutiplyBlock(A, B, C, i, j);
//
//
//				}
//					#pragma omp section  // 一个线程用来移动A的上半部分 和 B的左半部分
//					{
//						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num() );
//						i = 2;
//						for (j = 0; j < 4; j++)
//							MutiplyBlock(A, B, C, i, j);
//					}
//
//					#pragma omp section // 一个线程用来移动A的下半部分 和 B的右半部分
//					{
//						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num() );
//						i = 3;
//						for (j = 0; j < 4; j++)
//							MutiplyBlock(A, B, C, i, j);
//
//
//					}
//				}
//			}
//			MoveDataBlockLine_A(A, 0);
//			MoveDataBlockLine_B(B, 0);
//			MoveDataBlockLine_A(A, 1);
//			MoveDataBlockLine_B(B, 1);
//
//			MoveDataBlockLine_A(A, 2);
//			MoveDataBlockLine_B(B, 2);
//			MoveDataBlockLine_A(A, 3);
//			MoveDataBlockLine_B(B, 3);
//		}
//
//		// 显示本次迭代结果
//		/*fpC = fopen( "C.txt", "w+" );
//		for( i=0; i< MATRIX_MAX_SIZE; i++ )
//		for( j=0; j< MATRIX_MAX_SIZE; j++ )
//		{
//		//printf( "\n C[%d][%d] = %d ", i, j, C[i][j] );
//		fprintf( fpC, "\n C[%d][%d] = %d ", i, j, C[i][j] );
//		}
//		fclose(fpC);*/
//	}
//	else
//		cout << "\n Error!  MATRIX_MAX_SIZE % 4 != 0 ! " << endl;
//
//	cout<<"\nCannon 计算完毕 ...\n"<<endl;
//
//}
				//*************END****************//
//*******************Cannon并行矩阵乘算法************************//


//****************************行分块矩阵乘并行算法**************************************//
					//*************START****************//		
void mul(double matrix1[][MATRIX_MAX_SIZE],int row1,int col1, double matrix2[][MATRIX_MAX_SIZE],int row2,int col2, double matrix[][MATRIX_MAX_SIZE])
{
	int i, j, k;
	for (i = 0; i<col1; i++)
	{
		for (j = 0; j< row2; j++)
		{
			matrix[i][j] = 0;
		}
	}
#ifdef PARALLEL
	// 设置 omp 线程数
	omp_set_num_threads(4);
	#pragma omp parallel for shared(matrix,matrix1,matrix2) private(i,j,k)
	for (i = 0; i< row1; i++)
	{
		for (j = 0; j< col2; j++)
		{
			for (k = 0; k<col1; k++)
			{
				matrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
#else
	for (i = 0; i< row1; i++)
	{
		for (j = 0; j< col2; j++)
		{
			for (k = 0; k< col1; k++)
			{
				matrix[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}
#endif
}
						//*************END****************//
//****************************行分块矩阵乘并行算法**********************************//

int get_start_row(int k, int start_index)
{
	int res;
	if (k < start_index)
		res = start_index;
	else if (k == start_index)
		res = start_index + 4;
	else if ((k - start_index) % 4 == 0)
		res = k + 4;
	else
		res = ((k - start_index) / 4 + 1) * 4 + start_index;
	return res;
}

//卷帘分块高斯消元法，用于矩阵行列式，矩阵秩，矩阵的上三角矩阵，矩阵逆的并行化算法
//此算法在论文的3.4.2节
void gauss_elim(double a[][ MATRIX_MAX_SIZE], int k, int start_index)
{
	int i, j, start, n;
	n =  MATRIX_MAX_SIZE;
	start = get_start_row(k, start_index);
	for (i = start; i<n; i += 4)
	{
		a[i][k] = a[i][k] / a[k][k];//乘子，消去第i行的k列        
	}
	for (i = start; i<n; i += 4)
	{
		//对k+1...n-1行
		for (j = k + 1; j<n; j++)
		{
			//对k+1...n-1列
			a[i][j] = a[i][j] - a[i][k] * a[k][j];//消元         
		}
		//printf("\nthread number: %d,%d ", omp_get_thread_num(),i);
	}
}
//LU分解
void lu(double a[][MATRIX_MAX_SIZE], int rank, double l[][MATRIX_MAX_SIZE], double u[][MATRIX_MAX_SIZE])
{
	int i, j, k;
	int m = 1;
	//初始化，l的对角线元素必然是1
	for (i = 0; i < rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			u[i][j] = 0;
			if (i == j)
				l[i][j] = 1;
			else
				l[i][j] = 0;
		}
	}
#ifdef MATRIX_DEBUG
	for (i = 0; i<rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			cout << a[i][j] << " ";
		}
		cout << endl;
	}
#endif
	for (k = 0; k < rank; k++)
	{
		if (a[k][k] == 0)
		{
			cout << "不能进行直接三角LU分解" << endl;
			break;
		}
#ifdef PARALLEL
		//并行进行LU分解
		omp_set_num_threads(4);
		int start_index;
		#pragma omp parallel shared( a,k ) private(start_index)
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 0;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 1;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 2;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 3;
					gauss_elim(a, k, start_index);
				}
			}
		}
#else
		for (i = k + 1; i<rank; i++)
		{
			a[i][k] = a[i][k] / a[k][k];//乘子，消去第i行的k列        
		}
		for (i = k + 1; i<rank; i++)
		{
			//对k+1...n-1行
			for (j = k + 1; j<rank; j++)
			{
				//对k+1...n-1列
				a[i][j] = a[i][j] - a[i][k] * a[k][j];//消元         
			}
		}
#endif
	}
	for (i = 0; i<rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			if (j<i)
				l[i][j] = a[i][j];//下三角矩阵
			else
				u[i][j] = a[i][j];//上三角矩阵
		}
	}
#ifdef MATRIX_DEBUG
	cout << "下三角矩阵 L：" << endl;
	for(i = 0; i < rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			cout << l[i][j]<<" ";
		}
		cout << endl;
	}
	cout << "上三角矩阵 U："<< endl;
	for (i = 0; i < rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			cout << u[i][j]<<" ";
		}
		cout << endl;
	}
	cout << endl;
	double(*res3)[ MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
	mul(l,rank,rank,u,rank,rank,res3);
	
	for (i = 0; i<rank; i++)
	{
		for (j = 0; j<rank; j++)
		{
			cout << res3[i][j] << " ";
		}
		cout << endl;
	}
#endif
}
//求一个方阵的上三角矩阵
void get_triU(double a[][ MATRIX_MAX_SIZE], int rank)
{
	int i, j, k, n;
	n =  rank;
#ifdef MATRIX_DEBUG
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			cout << a[i][j] << " ";
		}
	cout << endl;
	}
	cout << endl;
#endif
	for (k = 0; k<n; k++)
	{
		double max = a[k][k];
		int index = k;
		for (i = k + 1; i < n; i++)
		{
			if (abs(a[i][k])>max)
			{
				max = a[i][k];
				index = i;
			}
		}
		int m = 0;
		if (a[index][k] == 0)
			continue;
		else if (index != k)
		{
			double(*tmp) = new double[ MATRIX_MAX_SIZE];
			for (j = 0; j < n; j++)
				tmp[j] = a[k][j];
			for (j = 0; j < n; j++)
				a[k][j] = a[index][j];
			for (j = 0; j < n; j++)
				a[index][j] = tmp[j];
		}
#ifdef PARALLEL
		//并行
		omp_set_num_threads(4);
		int start_index;
		#pragma omp parallel shared( a,k ) private(start_index)
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 0;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 1;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 2;
					gauss_elim(a, k, start_index);
				}
				#pragma omp section
				{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 3;
				gauss_elim(a, k, start_index);
				}
			}
		}
#else
		//串行
		for (i = k + 1; i<n; i++)
		{
			a[i][k] = a[i][k] / a[k][k];//乘子，消去第i行的k列        
		}
		for (i = k + 1; i<n; i++)
		{
			//对k+1...n-1行
			for (j = k + 1; j<n; j++)
			{
				//对k+1...n-1列
				a[i][j] = a[i][j] - a[i][k] * a[k][j];//消元         
			}
		}
#endif
	}
	for (i = 0; i<n; i++)
	{
		for (j = 0; j<n; j++)
		{
			if (j<i)
				a[i][j] = 0;
		}
	}
#ifdef MATRIX_DEBUG
	for(i = 0; i<n; i++)
	{
		for(j = 0; j<n; j++)
		{
			cout<<a[i][j]<<" ";
		}
		cout<<endl;
	}
#endif
}
//行列式求解
double det(double matrix[][ MATRIX_MAX_SIZE], int rank)
{
	int i, j, k, n;
	n = rank;
	double res = 1;
	int ratio = 1;
	double(*matrix1)[ MATRIX_MAX_SIZE] = new double[ rank][ MATRIX_MAX_SIZE];
	for (i = 0; i < rank; i++)
	{
		for (j = 0; j < rank; j++)
			matrix1[i][j] = matrix[i][j];
	}
	//列主元高斯消元法
	int start_index;
	for (k = 0; k < n; k++)
	{
		//选主元
		double max = matrix1[k][k];
		int index = k;
		for (i = k + 1; i < n; i++)
		{
			if (abs(matrix1[i][k])>max)
			{
				max = matrix1[i][k];
				index = i;
			}
		}
		if (index != k)
		{
			ratio = -ratio;
			int tmp[ MATRIX_MAX_SIZE] = { 0 };
			for (j = 0; j < n; j++)
				tmp[j] = matrix1[k][j];
			for (j = 0; j < n; j++)
				matrix1[k][j] = matrix1[index][j];
			for (j = 0; j < n; j++)
				matrix1[index][j] = tmp[j];
		}
        if (matrix1[k][k] == 0)
		{
			res = 0;
			break;
		}
#ifdef PARALLEL
		//并行方式（卷帘分块）
		omp_set_num_threads(4);
		#pragma omp parallel shared( matrix1,k ) private(start_index)
		{
			#pragma omp sections
			{
				#pragma omp section
				{
					#ifdef MATRIX_DEBUG
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					#endif
					start_index = 0;
					gauss_elim(matrix1, k, start_index);
				}
				#pragma omp section
				{
					#ifdef MATRIX_DEBUG
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					#endif
					start_index = 1;
					gauss_elim(matrix1, k, start_index);
				}
				#pragma omp section
				{
					#ifdef MATRIX_DEBUG
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					#endif
					start_index = 2;
					gauss_elim(matrix1, k, start_index);
				}
				#pragma omp section
				{
					#ifdef MATRIX_DEBUG
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					#endif
					start_index = 3;
					gauss_elim(matrix1, k, start_index);
				}
			}
		}
#else
			//串行方式
			for (i = k + 1; i<n; i++)
			{
				matrix1[i][k] = matrix1[i][k] / matrix1[k][k];//乘子，消去第i行的k列        
			}
			for (i = k + 1; i<n; i++)
			{
				//对k+1...n-1行
				for (j = k + 1; j<n; j++)
				{
					//对k+1...n-1列
					matrix1[i][j] -= matrix1[i][k] * matrix1[k][j];//消元         
				}
			}
#endif
	}
	for (i = 0; i<n; i++)
	{
		res *= matrix1[i][i];
	}
	res *= ratio;
	return res;
}

//检查是否是三角矩阵
Tri_matrix_type check_if_tri_matrix(double matrix[][MATRIX_MAX_SIZE], int rank)
{
	// 0表示不是三角矩阵，1表示是上三角矩阵，2表示是下三角矩阵，3表示对角阵
	int i, j;
	bool flag_u = true;
	bool flag_l = true;
	bool flag = false;
	for (i = 0; i < rank; i++)
	{
		for (j = 0; j < rank; j++)
		{
			if (i>j)
			{
				if (matrix[i][j] != 0)
				{
					flag_u = false;
				}
			}
			else if (i < j)
			{
				if (matrix[i][j] != 0)
				{
					flag_l = false;
				}
			}
			if (!flag_l&&!flag_u)
			{
				flag = true;
			}
		}
		if (flag)
			break;
	}
	if (flag_l&&flag_u)
		return Diag_matrix;
	else if (flag_l)
		return TriL_matirx;
	else if (flag_u)
		return TriU_matirx;
	else
		return Not_tri_matrix;
}

//求下三角矩阵的逆矩阵
//此算法在论文的3.3.1节
void invese_matrix_triL(double matrix[][ MATRIX_MAX_SIZE], int rank, double matrix_I[][ MATRIX_MAX_SIZE])
{
	int i, j, k;
	int n = rank;
#ifdef PARALLEL
	omp_set_num_threads(4);
	#pragma omp parallel for shared( matrix,matrix_I) private( i,j,k )
	for (i = 0; i<n; i++)
	{
		matrix_I[i][i] = 1 / matrix[i][i]; //对角元素的值，直接取倒数，这里为1
		for (k = i + 1; k<n; k++)
		{
			double s = 0;
			for (j = 0; j < k; j++)
				s += matrix[k][j] * matrix_I[j][i];
			matrix_I[k][i] = -s / matrix[k][k];   //迭代计算，按列顺序依次得到每一个值
		}
	}
#else
	for (i = 0; i<n; i++)
	{
		if (matrix[i][i] == 0)
		{
			cout << "奇异矩阵，不存在逆矩阵" << endl;
			break;
		}
		matrix_I[i][i] = 1 / matrix[i][i]; //对角元素的值，直接取倒数，这里为1
		for (k = i + 1; k<n; k++)
		{
			double s = 0;
			for (j = 0; j < k; j++)
				s += matrix[k][j] * matrix_I[j][i];
			matrix_I[k][i] = -s / matrix[k][k];   //迭代计算，按列顺序依次得到每一个值
		}
	}
#endif
}
//求上三角矩阵的逆矩阵
//此算法在论文的3.3.1节
void invese_matrix_triU(double matrix[][ MATRIX_MAX_SIZE], int rank, double matrix_I[][ MATRIX_MAX_SIZE])
{
	int i, j, k;
	int n = rank;
#ifdef PARALLEL
	omp_set_num_threads(4);
	#pragma omp parallel for shared( matrix,matrix_I) private( i,j,k )
	for (i = 0; i<n; i++) /*求矩阵U的逆 */
	{
		/*if (matrix[i][i] == 0)
		{
		cout << "奇异矩阵，不存在逆矩阵" << endl;
		break;
		}*/
		matrix_I[i][i] = 1 / matrix[i][i];//对角元素的值，直接取倒数

		for (k = i - 1; k >= 0; k--)
		{
			double s = 0;
			for (j = k + 1; j <= i; j++)
				s = s + matrix[k][j] * matrix_I[j][i];
			matrix_I[k][i] = -s / matrix[k][k];//迭代计算，按列倒序依次得到每一个值，
		}
	}
#else
	for (i = 0; i<n; i++) /*求矩阵U的逆 */
	{
		if (matrix[i][i] == 0)
		{
			cout << "奇异矩阵，不存在逆矩阵" << endl;
			break;
		}
		matrix_I[i][i] = 1 / matrix[i][i];//对角元素的值，直接取倒数

		for (k = i - 1; k >= 0; k--)
		{
			double s = 0;
			for (j = k + 1; j <= i; j++)
				s = s + matrix[k][j] * matrix_I[j][i];
			matrix_I[k][i] = -s / matrix[k][k];//迭代计算，按列倒序依次得到每一个值，
		}
	}
#endif
}

//矩阵求逆（基于LU分解）
//在论文的3.4.3节
void inverse(double A[][MATRIX_MAX_SIZE], int rank, double a[][MATRIX_MAX_SIZE])
{
	//检查是否是上（下）三角矩阵
	int i, j, k, n = rank;

	double(*A1)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
	double(*L)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
	double(*U)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];

	double(*l)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
	double(*u)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
	double mm = 1;
	//初始化
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
		{
			A1[i][j] = A[i][j];
			a[i][j] = 0;
			l[i][j] = 0;
			u[i][j] = 0;
		}
	}
	Tri_matrix_type type = check_if_tri_matrix(A, rank);
	//原矩阵是对角矩阵
	if (type == Diag_matrix)
	{
		cout << "是对角矩阵" << endl;
		for (i = 0; i < n; i++)
		{
			if (A[i][i] == 0)
				cout << "奇异矩阵，不存在逆矩阵" << endl;
			a[i][i] = 1 / A[i][i];
		}
	}
	//原矩阵是上三角矩阵
	else if (type == TriU_matirx)
	{
		cout << "是上三角矩阵" << endl;

		double s;
		for (i = 0; i < n; i++) /*求矩阵U的逆 */
		{
			a[i][i] = 1 / A[i][i];//对角元素的值，直接取倒数
			if (A[i][i] == 0)
			{
				cout << "奇异矩阵，不存在逆矩阵" << endl;
				break;
			}
			for (k = i - 1; k >= 0; k--)
			{
				s = 0;
				for (j = k + 1; j <= i; j++)
					s = s + A[k][j] * a[j][i];
				a[k][i] = -s / A[k][k];//迭代计算，按列倒序依次得到每一个值，
			}
		}
	}
	//原矩阵是下三角矩阵
	else if (type == TriL_matirx)
	{
		cout << "是下三角矩阵" << endl;
		for (i = 0; i < n; i++)
		{
			if (A[i][i] == 0)
			{
				cout << "奇异矩阵，不存在逆矩阵" << endl;
				break;
			}
			a[i][i] = 1 / A[i][i]; //对角元素的值，直接取倒数，这里为1
			for (k = i + 1; k < n; k++)
			{
				double s = 0;
				for (j = 0; j < k; j++)
					s += A[k][j] * a[j][i];
				a[k][i] = -s / A[k][k];   //迭代计算，按列顺序依次得到每一个值
			}
		}
	}
	//原矩阵是普通矩阵
	else
	{
		//初始化L,U矩阵
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				U[i][j] = 0;
				if (i == j)
					L[i][j] = 1;
				else
					L[i][j] = 0;
			}
		}
		//记录交换信息，以帮助生成矩阵P
		//由PA=LU推导出A的逆矩阵
		int(*record_swap) = new int[2 * rank];
		//单位矩阵P
		matrix P;
		matrix_resize(P, rank, rank);
		matrix_eye(P, rank);
		int m = 0;
		//列主元高斯消元法
		for (k = 0; k < n; k++)
		{
			double max = A[k][k];
			int index = k;
			for (i = k + 1; i < n; i++)
			{
				if (abs(A[i][k])>max)
				{
					max = A[i][k];
					index = i;
				}
			}
			if (index != k)
			{
				//记录交换信息
				record_swap[m++] = k;
				record_swap[m++] = index;
				double(*tmp) = new double[MATRIX_MAX_SIZE];
				for (j = 0; j < n; j++)
					tmp[j] = A[k][j];
				for (j = 0; j < n; j++)
					A[k][j] = A[index][j];
				for (j = 0; j < n; j++)
					A[index][j] = tmp[j];
			}
			if (A[k][k] == 0)
			{
				cout << "奇异矩阵，不存在逆矩阵" << endl;
				break;
			}
#ifdef PARALLEL
			//并行
			int start_index;
			#pragma omp parallel shared( A,k ) private(start_index)
			{
				#pragma omp sections
				{
					#pragma omp section
					{
						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
						start_index = 0;
						gauss_elim(A, k, start_index);
					}
					#pragma omp section
					{
					//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
					start_index = 1;
					gauss_elim(A, k, start_index);
				}
					#pragma omp section
					{
						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
						start_index = 2;
						gauss_elim(A, k, start_index);
					}
					#pragma omp section
					{
						//printf("\n Setctons 2, thread number %d ", omp_get_thread_num());
						start_index = 3;
						gauss_elim(A, k, start_index);
					}
				}
			}
#else
			//串行
			for (i = k + 1; i < n; i++)
			{
				A[i][k] = A[i][k] / A[k][k];//乘子，消去第i行的k列        
			}
			for (i = k + 1; i < n; i++)
			{
				//对k+1...n-1行
				for (j = k + 1; j < n; j++)
				{
					//对k+1...n-1列
					A[i][j] = A[i][j] - A[i][k] * A[k][j];//消元         
				}
			}
#endif
		}
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				if (j < i)
					L[i][j] = A[i][j];//下三角矩阵
				else
					U[i][j] = A[i][j];//上三角矩阵
			}
		}
		double(*tmp) = new double[rank];
		for (i = 0; i < m; i += 2)
		{
			int k = record_swap[i];
			int index = record_swap[i + 1];
			for (j = 0; j < n; j++)
				tmp[j] = P.data[k][j];
			for (j = 0; j < n; j++)
				P.data[k][j] = P.data[index][j];
			for (j = 0; j < n; j++)
				P.data[index][j] = tmp[j];
		}
#ifdef MATRIX_DEBUG
		mul(L, rank, rank, U, rank, rank, a);
		cout << "L:" << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << L[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		cout << "U=:" << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << U[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		cout << "L*U=:" << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << a[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
#endif

		//求L和U的逆矩阵
		invese_matrix_triL(L, rank, l);
		invese_matrix_triU(U, rank, u);

#ifdef MATRIX_DEBUG
		cout << "验证L的逆矩阵是否是l" << endl;
		double(*res3)[MATRIX_MAX_SIZE] = new double[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
		mul(L, rank, rank, l, rank, rank, res3);
		cout << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << res3[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		cout << "验证U的逆矩阵是否是u" << endl;
		double(*res4)[MATRIX_MAX_SIZE] = new double[rank][MATRIX_MAX_SIZE];
		mul(U, rank, rank, u, rank, rank, res4);
		cout << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << res4[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
#endif

		double(*res1)[MATRIX_MAX_SIZE] = new double[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
		mul(u, rank, rank, l, rank, rank, res1);
		mul(res1, rank, rank, P.data, rank, rank, a);

#ifdef MATRIX_DEBUG
		cout << "逆矩阵a:" << endl;
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << a[i][j] << " ";
			}
			cout << endl;
		}
		cout << endl;
		double(*res2)[MATRIX_MAX_SIZE] = new double[MATRIX_MAX_SIZE][MATRIX_MAX_SIZE];
		cout << "验证结果:" << endl;
		mul(A1, rank, rank, a, rank, rank, res2);
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				cout << res2[i][j] << " ";
			}
			cout << endl;
		}
#endif
	}
}
