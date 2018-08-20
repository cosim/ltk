#include "stdafx.h"
#include "NetworkBuffer.h"
#include "Common.h"
#include <cassert>

using std::shared_ptr;

NetworkBuffer::NetworkBuffer(size_t capacity)
{
	m_buffer = new char[capacity];
	m_capacity = capacity;
	m_offset = 0;
	m_length = 0;
}

NetworkBuffer::NetworkBuffer(const char *data, size_t length)
{
	m_buffer = new char[length];
	memcpy(m_buffer, data, length);
	m_offset = 0;
	m_capacity = length;
	m_length = length;
}


NetworkBuffer::~NetworkBuffer()
{
	delete[] m_buffer;
}

shared_ptr<NetworkBuffer> NetworkBuffer::Create(size_t capacity)
{
    shared_ptr<NetworkBuffer> ptr;
    ptr.reset(new NetworkBuffer(capacity));
    return std::move(ptr);
}

shared_ptr<NetworkBuffer> NetworkBuffer::Clone()
{
	shared_ptr<NetworkBuffer> ptr;
	ptr.reset(new NetworkBuffer(m_length));
	memcpy(ptr->m_buffer, m_buffer, m_length);
	ptr->m_length = m_length;
	return ptr;
}

int NetworkBuffer::ReadInt32(long &data)
{
	if (m_offset + 4 > m_length)
	{
        data = 0;
		return eBufferOverflow;
	}
	data = *(long*)(m_buffer + m_offset);
	m_offset += 4;

	return eBufferOk;
}

int NetworkBuffer::ReadByte(unsigned char &data)
{
    if (m_offset + 1 > m_length)
    {
        data = 0;
        return eBufferOverflow;
    }
    data = *(unsigned char *)(m_buffer + m_offset);
    m_offset += 1;

    return eBufferOk;
}

int NetworkBuffer::ReadInt64(long long &data)
{
    if (m_offset + 8 > m_length)
    {
        data = 0;
        return eBufferOverflow;
    }
    data = *(long long*)(m_buffer + m_offset);
    m_offset += 8;

    return eBufferOk;
}

int NetworkBuffer::ReadDouble(double &data)
{
    if (m_offset + 8 > m_length)
    {
        data = 0;
        return eBufferOverflow;
    }
    data = *(double*)(m_buffer + m_offset);
    m_offset += 8;

    return eBufferOk;
}

int NetworkBuffer::ReadString(std::string &data, size_t limit)
{
    if (m_offset + 4 > m_length)
	{
        data = "";
		return eBufferOverflow;
	}
	size_t len = *(long *)(m_buffer + m_offset); 

	if (len > limit)  // 这里要限制最大长度以防拒绝服务攻击 比如设置了字符串长度为 2GB
	{
        data = "";
        return eBufferOverflow;
	}
    if (m_offset + 4 + len > m_length)
	{
        data = "";
        return eBufferOverflow;
	}
    data.assign(m_buffer + m_offset + 4, len);
	m_offset = m_offset + 4 + len;

	return eBufferOk;
}

int NetworkBuffer::GetDataLength(long &length)
{
    if (m_offset + 4 > m_length)
    {
        length = 0;
        return eBufferOverflow;
    }
    length = *(long *)(m_buffer + m_offset);
    return eBufferOk;
}

int NetworkBuffer::ReadData(char *data, size_t length)
{
    if (m_offset + 4 > m_length)
    {
        memset(data, 0, length);
        return eBufferOverflow;
    }
    size_t len = *(long *)(m_buffer + m_offset);

    if (len > length)
    {
        memset(data, 0, length);
        return eBufferOverflow;
    }
    if (m_offset + 4 + len > m_length)
    {
        memset(data, 0, length);
        return eBufferOverflow;
    }
    memcpy(data, m_buffer + m_offset + 4, len);
    m_offset = m_offset + 4 + len;

    return eBufferOk;
}

shared_ptr<NetworkBuffer> NetworkBuffer::ReadNestedBuffer()
{
    shared_ptr<NetworkBuffer> ptr;
    long len = 0;
    this->GetDataLength(len);
    if (len > 0)
    {
        ptr.reset(new NetworkBuffer(len));
        this->ReadData(ptr->GetData(), ptr->GetCapacity());
        ptr->SetLength(len);
    }
    return ptr;
}

int NetworkBuffer::WriteByte(unsigned char b)
{
    EnsureSpace(1);
    *(unsigned char *)(m_buffer + m_offset) = b;
    m_offset += 1;

    if (m_length < m_offset)
        m_length = m_offset;

    return eBufferOk;
}

int NetworkBuffer::WriteInt32(long data)
{
	EnsureSpace(4);
	*(long *)(m_buffer + m_offset) = data;
	m_offset += 4;

	if (m_length < m_offset)
		m_length = m_offset;

	return eBufferOk;
}

int NetworkBuffer::WriteDouble(double data)
{
    EnsureSpace(8);
    *(double *)(m_buffer + m_offset) = data;
    m_offset += 8;

    if (m_length < m_offset)
        m_length = m_offset;

    return eBufferOk;
}

int NetworkBuffer::WriteInt64(long long data)
{
    EnsureSpace(8);
    *(long long *)(m_buffer + m_offset) = data;
    m_offset += 8;

    if (m_length < m_offset)
        m_length = m_offset;

    return eBufferOk;
}

int NetworkBuffer::WriteString(const std::string &data)
{
	EnsureSpace(4 + data.length());

	*(long *)(m_buffer + m_offset) = (long)data.length();
	memcpy(m_buffer + m_offset + 4, &data[0], data.length());
	m_offset = m_offset + 4 + data.length();

	if (m_length < m_offset)
		m_length = m_offset;
	
	return eBufferOk;
}

int NetworkBuffer::WriteData(const char *data, size_t length)
{
    EnsureSpace(4 + length);

    *(long *)(m_buffer + m_offset) = (long)length;
    memcpy(m_buffer + m_offset + 4, data, length);
    m_offset = m_offset + 4 + length;

    if (m_length < m_offset)
        m_length = m_offset;

    return eBufferOk;
}

void NetworkBuffer::EnsureSpace(size_t size)
{
	while (m_offset + size > m_capacity)
	{
		m_capacity = m_capacity * 2;
		char *old_buf = m_buffer;
		m_buffer = new char[m_capacity];
		memcpy(m_buffer, old_buf, m_offset); // ???
		delete[] old_buf;
	}
}

char *NetworkBuffer::GetData()
{
	return m_buffer;
}

size_t NetworkBuffer::GetCapacity()
{
	return m_capacity;
}

size_t NetworkBuffer::GetLength()
{
	return m_length;
}

void NetworkBuffer::SetLength(size_t length)
{
	assert(length <= m_capacity);
	m_length = length;
}

void NetworkBuffer::Rewind()
{
	m_offset = 0;
}

void NetworkBuffer::Encrypt()
{
    XorObfuscate();
}

void NetworkBuffer::Decrypt()
{
    XorObfuscate();
}

void NetworkBuffer::XorObfuscate()
{
    assert(0 != m_length);
    for (size_t i = 0; i < m_length; i++)
    {
        m_buffer[i] = m_buffer[i] ^ 0xAA;
    }
}

int NetworkBuffer::Tell()
{
    return m_offset;
}

void NetworkBuffer::Seek(int offset)
{
    assert(offset >= 0 && offset <= m_length);
    m_offset = offset;
}
