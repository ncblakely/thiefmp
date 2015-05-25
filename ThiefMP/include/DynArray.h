#pragma once

class cDABaseSrvFns
{
public:
	cDABaseSrvFns()
	{
		m_pData = NULL;
		m_size = NULL;
	}

	static int DoResize(void**, unsigned int, unsigned int);

protected:
	void* m_pData;
	unsigned long m_size;
};

template <typename T> class cDynArray : public cDABaseSrvFns
{
public:

	cDynArray() : cDABaseSrvFns() { }
	cDynArray(unsigned int P1) : cDABaseSrvFns()
	{
		if (P1)
		{
			DoResize(&m_pData, sizeof(T), P1);
			m_allocSize = P1;
		}
	}

	operator bool () { return m_pData != 0; }

	T& operator[](unsigned int index)
	{
#ifdef _DEBUG
		//if (index >= m_size)
		//	_ASSERTE(false && "Index out of range");
#endif

		return reinterpret_cast<T*>(m_pData)[index];
	}

	const T& operator[](unsigned int index) const
	{
		if (index >= m_size)
			_ASSERTE(false && "Index out of range");

		return reinterpret_cast<T*>(m_pData)[index];
	}

	unsigned int Append(const T& rt)
	{
		//uint index = m_size + 1;
		//if (index > m_allocSize)
		//{
		//	m_allocSize++;
		//	DoResize(&m_pData, sizeof(T), m_allocSize);
		//}

		//m_size = index--;
		//((T*)m_pData)[index] = rt;
		//
		//return index;
	}

	T& GetIndex(unsigned int index)
	{
#ifdef _DEBUG
		//if (index >= m_size)
		//	_ASSERTE(false && "Index out of range");
#endif

		return reinterpret_cast<T*>(m_pData)[index];
	}

	//int SetSize(unsigned int newSize)
	//{
	//	if (m_size == newSize)
	//		return 0;


	//}

	unsigned long Size()
	{
		return m_size;
	}

	void Destroy()
	{
		if (m_pData)
		{
			DoResize(&m_pData, sizeof(T), NULL);
			m_pData = NULL;
			m_size = 0;
		}
	}
};