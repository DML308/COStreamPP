#ifndef _MATRIX_H_
#define _MATRIX_H_

//#define PARALLEL
#define MATRIX_DEBUG
#define MATRIX_MAX_SIZE 2000
//������
class matrix
{
public:
	//��һ����ά���鱣�����Ԫ��
	double(*data)[MATRIX_MAX_SIZE];
	//�������������
	int row, col;
	matrix() :row(0), col(0), data(NULL){}
	matrix& operator =(const matrix& b)//��ֵ�����
	{
		if (this != &b)
		{
			this->row = b.row;
			this->col = b.col;
			//��Ϊ�Ⱥ���ߵľ�������ռ䣬�����Ͳ��öԲ�������ߵľ���resize��
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
	/*~matrix()
	{
		int i;
		for (i = 0; i<row; i++)
			delete[MATRIX_MAX_SIZE] data[i];
		delete[] data;
		data = nullptr;
	}*/
};
//������
class Vector
{
public:
	//����������
	double *data;
	//������ά��
	int dim;
	Vector():data(NULL),dim(0){}
	Vector& operator =(const Vector& b)//��ֵ�����
	{
		if (this != &b)
		{
			this->dim = b.dim;
			//��Ϊ�Ⱥ���ߵ���������ռ䣬�����Ͳ��öԲ�������ߵ�����resize��
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
/***********************************�������******************************/
//����ռ䣬�����þ���������
//����Ǵ���һ�������������resize���紴��һ����λ����ֱ��
//matrix a;matrix_eye(a,rank);����
void matrix_resize(matrix& a, int, int);

//��ȡ������ĳ��λ�õ�Ԫ��
double matrix_get_num(matrix&, int, int);

//�޸ľ�����ĳ��λ�õ�Ԫ�ص�ֵ����ʱ������ʼ���������Ԫ�ص�ֵ
void matrix_set_num(matrix&, int, int, double);

//��ӡ���������Ԫ��
void matrix_print(matrix&);

//�Ѵ���ľ����Ϊһ����λ����
void matrix_eye(matrix&,int rank);

//�Ѵ���ľ����Ϊһ��ȫ0����
void matrix_zeros(matrix&,int,int);

//�Ѵ���ľ����Ϊһ��ȫ1����
void matrix_ones(matrix&,int,int);

//����matrix�������+,����ӷ�
matrix operator+(const matrix &a, const matrix &b);

//����matrix�������-,�������
matrix operator-(const matrix &a, const matrix &b);

//����matrix�������*������˷�
matrix operator*(const matrix &a, const matrix &b);

//����������Ԫ�ر�Ϊ���ǵĸ���
void matrix_opposite(matrix&);

//���������ʽ
double matrix_det(matrix&);

//�����LU�ֽ�
void matrix_LU(matrix& a, matrix& l, matrix u);

//����ת��
matrix matrix_transpose(matrix& );

//�����һ��
void matrix_norm(matrix&);

//��ȡһ������������Ǿ���
matrix matrix_get_triU(matrix&);

//������
matrix matrix_inverse(matrix&);

//����ˮƽ�ϲ�
matrix matrix_hstack(matrix&, matrix&);

//����ֱ�ϲ�
matrix matrix_vstack(matrix&, matrix&);

/***********************************�������******************************/


/***********************************��������******************************/
//�趨������ά��
void Vector_resize(Vector& a, int);

//����Vector�������+,�������
Vector operator+(const Vector &a, const Vector &b);

//����Vector�������-,��������
Vector operator-(const Vector &a, const Vector &b);

//��ȡ�����е�ĳ��ά�ȵ���
double Vector_get_num(Vector&, int);

//�޸������е�ĳ��ά�ȵ���
void Vector_set_num(Vector&, int,double);

//��ȡ�����ĳ��������
Vector matrix_get_row_vector(matrix& a, int target_row);

//��ȡ�����ĳ��������
Vector matrix_get_col_vector(matrix& a, int target_col);

//�������
double Vector_dot(Vector& a, Vector& b);

//��ȡ������
double Vector_sum(Vector& a);

//��ȡ������ĳ���������ĺ�
double matrix_sum_of_row_vector(matrix& a, int target_row);

//��ȡ������ĳ���������ĺ�
double matrix_sum_of_col_vector(matrix& a, int target_col);

//��ӡ����
void Vector_print(Vector&);
/***********************************��������******************************/
/*
//ʹ�ö�ά��������ʼ��matrix,��ҪCOSteam֧��new������������ʹ�ýϴ��ģ�ľ���ʱ��ʹջ�ռ����
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