#include <iostream>
#include <string.h>

using namespace std;

template <class T>
class RingBuffer
{
public:
	RingBuffer(int size,int len):m_buffer(NULL),m_size(size),m_len(len),m_writePos(0),m_readPos(0)
	{
		if(size&(size-1)!=0)
			m_size = roundup_power_of_two(size);
	}

	~RingBuffer()
	{
		if(m_buffer!=NULL)
		{
			delete[] m_buffer;
			m_buffer = NULL;
			m_size = 0;
			m_writePos = 0;
			m_readPos = 0;
		}
	}

	bool Initialize()
	{
		m_buffer = new T[m_size];
		if(!m_buffer)
			return false;

		m_writePos = 0;
		m_readPos = 0;

		return true;
	}

	unsigned int roundup_power_of_two(unsigned int val)
	{
		if(val&(val-1)==0)
			return val;

		unsigned int temp = 2;
		while(val>temp)
			temp = temp<<1;

		return temp;
	}

	unsigned int write(char *buffer)
	{
		int l=m_size-m_writePos+m_readPos;
		l = min(1,l);//是否有空间可写

		__sync_synchronize();

		memcpy((char*)&m_buffer[m_writePos&(m_size-1)],buffer,l*m_len);

		__sync_synchronize();

		m_writePos+=l;

		return l;
	}

	unsigned int read(char *buffer)
	{
		int l=m_writePos-m_readPos;
		l = min(1,l);//是否有空间可读

		__sync_synchronize();

		memcpy(buffer,(char*)&m_buffer[m_readPos&(m_size-1)],l*m_len);

		__sync_synchronize();

		m_readPos+=l;

		return l;
	}

private:
	T *m_buffer;
	unsigned int m_size;
	unsigned int m_writePos;
	unsigned int m_readPos;
	unsigned int m_len;
};