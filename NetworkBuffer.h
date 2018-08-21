#pragma once

#include <string>
#include <memory>

class NetworkBuffer
{
public:

	enum {
		eBufferOk = 0,
		eBufferOverflow = -1,
		eBufferTooLarge = -2
	};

	explicit NetworkBuffer(size_t capacity = 8 * 2014);

	NetworkBuffer(const char *data, size_t length);

    NetworkBuffer(const NetworkBuffer&) = delete;
    void operator=(const NetworkBuffer&) = delete;

	~NetworkBuffer();

    static std::shared_ptr<NetworkBuffer> Create(size_t capacity = 8 * 2014);

    std::shared_ptr<NetworkBuffer> Clone();

	int ReadInt32(long &data);

    int ReadByte(unsigned char &b);

    int ReadInt64(long long &data);

    int ReadDouble(double &data);

	int ReadString(std::string &data, size_t limit = 8 * 1024);

    int GetDataLength(long &length);

    int GetDataPointer(long &length, const char *&ptr);

    int ReadData(char *data, size_t length);

	std::shared_ptr<NetworkBuffer> ReadNestedBuffer();

	int WriteInt32(long data);

    int WriteByte(unsigned char b);

    int WriteInt64(long long data);

    int WriteDouble(double data);

	int WriteString(const std::string &data);

    int WriteData(const char *data, size_t length);

	int WriteNestedBuffer(const NetworkBuffer &buffer);

    size_t Tell();

    void Seek(size_t offset);

	char *GetData();
	size_t GetCapacity();  // 可以存放的数据大小 用于接收网络数据

	size_t GetLength(); // 数据真实的大小 写入的
	void SetLength(size_t length);

	void Rewind();

    void Encrypt();
    void Decrypt();

private:
	void EnsureSpace(size_t);
    void XorObfuscate();

private:
	char *m_buffer;
	size_t m_offset;
	size_t m_capacity;
	size_t m_length;
};

