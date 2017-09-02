#pragma once

#include "geometry.h"
#include <boost/utility/string_ref.hpp>
#include <fstream>
#include <memory>
#include <vector>

struct SDL_RWops;

class SaveFile
{
public:
    SaveFile(boost::string_ref filePath, bool writable);
    void writeInt8(int8_t value) { writeInt8(uint8_t(value)); }
    void writeInt8(uint8_t value);
    void writeInt16(int16_t value) { writeInt16(uint16_t(value)); }
    void writeInt16(uint16_t value);
    void writeInt32(int32_t value) { writeInt32(uint32_t(value)); }
    void writeInt32(uint32_t value);
    void writeInt64(int64_t value) { writeInt64(uint64_t(value)); }
    void writeInt64(uint64_t value);
    void write(bool value);
    void write(double value);
    void write(Vector2 value);
    void write(Vector3 value);
    void write(boost::string_ref value);
    void write(const std::string& value) { write(boost::string_ref(value)); }
    template<typename T>
    void write(const std::vector<T>& vector);

    int8_t readInt8() const { return int8_t(readUint8()); }
    uint8_t readUint8() const;
    int16_t readInt16() const { return int16_t(readUint16()); }
    uint16_t readUint16() const;
    int32_t readInt32() const { return int32_t(readUint32()); }
    uint32_t readUint32() const;
    int64_t readInt64() const { return int64_t(readUint64()); }
    uint64_t readUint64() const;
    bool readBool() const;
    double readDouble() const;
    std::string readString() const;
    Vector2 readVector2() const;
    Vector3 readVector3() const;
    template<typename T>
    void read(std::vector<T>& vector) const;

private:
    template<typename T>
    void write(const std::unique_ptr<T>& value) { value->save(*this); }
    template<typename T>
    void write(const T& value) { value.save(*this); }
    template<typename T>
    T read() const { return T::load(*this); }

    std::unique_ptr<SDL_RWops, void (&)(SDL_RWops*)> file;
};

template<typename T>
void SaveFile::write(const std::vector<T>& vector)
{
    writeInt32(int32_t(vector.size()));

    for (auto& element : vector)
        write(element);
}

template<typename T>
void SaveFile::read(std::vector<T>& vector) const
{
    auto size = readInt32();
    assert(vector.empty());
    vector.reserve(size_t(size));

    for (int i = 0; i < size; ++i)
        vector.push_back(read<T>());
}

template<>
inline double SaveFile::read<double>() const
{
    return readDouble();
}
