#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <string.h>
#include <arpa/inet.h>
#include <iterator>
#include <stdexcept>

#include <constants.hpp>

class Encoder {
public:
    Encoder() : _position(0) {
        // Reserve enough space for the size of this buffer
        WriteInt(0);
    }

    void WriteBoolean(bool value) {
        Write((char*)&value, sizeof(bool));
    }

    void WriteByte(char value) {
        Write((char*)&value, sizeof(char));
    }

    void WriteShort(short value) {
        value = htons(value);
        Write((char*)&value, sizeof(short));
    }

    void WriteInt(int value) {
        value = htonl(value);
        Write((char*)&value, sizeof(int));
    }

    void WriteFloat(float value) {
        uint32_t asInt;
        static_assert(sizeof(float) == sizeof(uint32_t), "Float must be 32-bit");

        // Copy float bits into an integer
        memcpy(&asInt, &value, sizeof(float));

        // Convert to network byte order
        asInt = htonl(asInt);

        Write(reinterpret_cast<char*>(&asInt), sizeof(uint32_t));
    }

    void WriteString(const std::string& value) {
        short size = value.length();
        WriteShort(size);
        Write((char*)value.c_str(), size);
    }

    const char* buffer() const {
        // get the position and then copy to the front of the _buffer
        int length = htonl(_position);
        //std::cout << "length is: " << length << std::endl;
        memcpy((char*)_buffer.data(), &length, sizeof(int));
        return _buffer.data();
    }

    const int size() const {
        return _buffer.size();
    }

private:
    void Write(char *data, unsigned int size) {
        // Reserve space in the buffer to avoid reallocations
        _buffer.reserve(_buffer.size() + size);

        // Copy data into buffer using std::copy
        std::copy(data, data + size, std::back_inserter(_buffer));

        // Update position
        _position += size;
    }

private:
    int _position;
    std::vector<char> _buffer;
};

class Decoder {
public:
    Decoder(const char* data, int size) : _buffer(data, data + size), _position(0)
    { }

    void ReadBoolean(bool* value) {
        Read(reinterpret_cast<char*>(value), sizeof(bool));
    }

    void ReadByte(unsigned char* value) {
        Read(reinterpret_cast<char*>(value), sizeof(unsigned char));
    }

    void ReadShort(short* value) {
        Read(reinterpret_cast<char*>(value), sizeof(short));
        *value = ntohs(*value);
    }

    void ReadInt(int* value) {
        Read(reinterpret_cast<char*>(value), sizeof(int));
        *value = ntohl(*value);
    }

    void ReadFloat(float* value) {
        uint32_t asInt;
        Read(reinterpret_cast<char*>(&asInt), sizeof(uint32_t));

        // Convert from network byte order
        asInt = ntohl(asInt);

        // Copy bits back into the float
        memcpy(value, &asInt, sizeof(float));
    }

    void ReadString(std::string* value) {
        short size;
        ReadShort(&size);
        value->assign(&_buffer[_position], size);
        _position += size;
    }

private:
    void Read(char* data, unsigned int size) {
        if (_position + size > _buffer.size()) {
            throw std::runtime_error("Not enough data in buffer");
        }
        memcpy(data, &_buffer[_position], size);
        _position += size;
    }

private:
    std::vector<char> _buffer;
    int _position;
};

struct Message {
    virtual void encode(Encoder& encoder) = 0;
    virtual void decode(Decoder& decoder) = 0;
};

struct HelloMessage : public Message
{
    std::string text;
    int addA;
    int addB;
    bool solved;
    float test;

    virtual void encode(Encoder& encoder) override {
        encoder.WriteByte(constants::hello_id);
        encoder.WriteString(text);
        encoder.WriteInt(addA);
        encoder.WriteInt(addB);
        encoder.WriteBoolean(solved);
        encoder.WriteFloat(test);
    }

    virtual void decode(Decoder& decoder) override {
        decoder.ReadString(&text);
        decoder.ReadInt(&addA);
        decoder.ReadInt(&addB);
        decoder.ReadBoolean(&solved);
        decoder.ReadFloat(&test);
    }
};

struct ReplyMessage : public Message
{
    std::string text;
    int result;
    bool solved;
    float test;

    virtual void encode(Encoder& encoder) override {
        encoder.WriteByte(constants::reply_id);
        encoder.WriteString(text);
        encoder.WriteInt(result);
        encoder.WriteBoolean(solved);
        encoder.WriteFloat(test);
    }

    virtual void decode(Decoder& decoder) override {
        decoder.ReadString(&text);
        decoder.ReadInt(&result);
        decoder.ReadBoolean(&solved);
        decoder.ReadFloat(&test);
    }
};

#endif
