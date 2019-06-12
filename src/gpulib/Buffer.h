//class Buffer definition
#include <stdlib.h>
#ifndef _BUFFER_H
#define _BUFFER_H
template<typename T>
class Buffer{
public:
	Buffer(int size);
	int bufferSize;
	//T peek(int index);			//peak seems useless
	T& operator[](const size_t); 
	//析构函数~Buffer();
	~Buffer()
	{
		//delete	[]buffer;
	}
private:
	T* buffer;
};
//暂时放在头文件中，模板类的编译有问题
template<typename T>
Buffer<T>::Buffer(int size){//constructor    分配缓冲区地址
	bufferSize = size;
	buffer = (T*)malloc(bufferSize*sizeof(T));
}
template<typename T>
T& Buffer<T>::operator[](const size_t index){
	return buffer[index];					//是否需要保证下标操作读的数据不会超出tail?
}

//pop() 
/*template<typename T>
void Buffer<T>::pop(int popNum){
	headOffset = (headOffset +popNum)%bufferSize;
}*/


//析构函数
//template<typename T>
//Buffer<T>::~Buffer(){
//		delete[] buffer;
//}
#endif