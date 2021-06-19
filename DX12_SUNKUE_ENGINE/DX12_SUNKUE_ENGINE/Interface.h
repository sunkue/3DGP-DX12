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

class Collideable
{
public:
	BoundingOrientedBox GetOOBB()const { return m_OOBB; }
	void SetOOBB(const BoundingOrientedBox& OOBB) { m_OOBB = OOBB; }
protected:
	BoundingOrientedBox m_OOBB;
};