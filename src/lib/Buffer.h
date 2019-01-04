//class Buffer definition
#include <stdlib.h>
#ifndef _BUFFER_H
#define _BUFFER_H
#define CACHE_LINE_SIZE 64

template <typename T>
class Buffer
{
public:
	Buffer(int size, int copySize, int copyStartPos);
	int bufferSize;
	int copySize;
	int copyStartPos;
	int writePos;
	T &operator[](const size_t);
	void *GetCacheAlignedAddr(void *p);
	~Buffer()
	{
		free(begin);
	}
	void *begin;
	T *buffer;
};

template <typename T>
Buffer<T>::Buffer(int size, int copySize, int copyStartPos)
{ 
	this->bufferSize = size;
	this->copySize = copySize;
	this->copyStartPos = copyStartPos;
	this->begin = malloc((bufferSize * sizeof(T) / CACHE_LINE_SIZE + 2) * CACHE_LINE_SIZE);
	this->buffer = (T *)GetCacheAlignedAddr(begin);
	writePos = 0;
}
template <typename T>
T &Buffer<T>::operator[](const size_t index)
{
	return buffer[index]; //是否需要保证下标操作读的数据不会超出tail?
}

template <typename T>
void *Buffer<T>::GetCacheAlignedAddr(void *p)
{
	long m = CACHE_LINE_SIZE;
	long ret = (((long)p + m - 1) & (-m));
	return (void *)ret;
}
#endif