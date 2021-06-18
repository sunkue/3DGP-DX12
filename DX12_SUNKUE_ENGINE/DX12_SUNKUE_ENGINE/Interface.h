#pragma once


class Reference
{
public:
	Reference() = default;
	virtual ~Reference() = default;
	void AddRef() { ++m_reference; }
	void Release() { if (--m_reference == 0) delete this; }
	size_t GetReference()const { return m_reference; }
private:
	size_t m_reference{ 0 };
};