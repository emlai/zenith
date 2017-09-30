#include "savefile.h"
#include <SDL.h>
#include <cstring>
#include <stdexcept>

static void closeFile(SDL_RWops* file)
{
    SDL_RWclose(file);
}

SaveFile::SaveFile(boost::string_ref filePath, bool writable)
:   file(SDL_RWFromFile(filePath.to_string().c_str(), writable ? "wb" : "rb"), closeFile)
{
    if (!file)
        throw std::runtime_error(SDL_GetError());
}

SaveFile::SaveFile(std::vector<char> buffer)
:   buffer(std::move(buffer)),
    file(SDL_RWFromMem(this->buffer.data(), this->buffer.size()), closeFile)
{
    if (!file)
        throw std::runtime_error(SDL_GetError());
}

uint64_t SaveFile::getSize() const
{
    auto size = SDL_RWsize(file.get());

    if (size < 0)
        throw std::runtime_error(SDL_GetError());

    return uint64_t(size);
}

int64_t SaveFile::getOffset() const
{
    return SDL_RWtell(file.get());
}

void SaveFile::seek(int64_t offset)
{
    SDL_RWseek(file.get(), offset, RW_SEEK_SET);
}

void SaveFile::writeInt8(uint8_t value)
{
    SDL_WriteU8(file.get(), value);
}

uint8_t SaveFile::readUint8() const
{
    return SDL_ReadU8(file.get());
}

void SaveFile::writeInt16(uint16_t value)
{
    SDL_WriteLE16(file.get(), value);
}

uint16_t SaveFile::readUint16() const
{
    return SDL_ReadLE16(file.get());
}

void SaveFile::writeInt32(uint32_t value)
{
    SDL_WriteLE32(file.get(), value);
}

uint32_t SaveFile::readUint32() const
{
    return SDL_ReadLE32(file.get());
}

void SaveFile::writeInt64(uint64_t value)
{
    SDL_WriteLE64(file.get(), value);
}

uint64_t SaveFile::readUint64() const
{
    return SDL_ReadLE64(file.get());
}

void SaveFile::write(bool value)
{
    writeInt8(uint8_t(value));
}

bool SaveFile::readBool() const
{
    return bool(readUint8());
}

void SaveFile::write(double value)
{
    uint64_t integer;
    static_assert(sizeof(value) == sizeof(integer), "");
    std::memcpy(&integer, &value, sizeof(integer));
    writeInt64(integer);
}

double SaveFile::readDouble() const
{
    auto integer = readUint64();
    double result;
    static_assert(sizeof(integer) == sizeof(result), "");
    std::memcpy(&result, &integer, sizeof(result));
    return result;
}

void SaveFile::write(boost::string_ref value)
{
    writeInt16(uint16_t(value.size()));

    for (auto ch : value)
        writeInt8(uint8_t(ch));
}

std::string SaveFile::readString() const
{
    auto size = readUint16();
    std::string string;
    string.reserve(size);

    for (int i = 0; i < size; ++i)
        string += char(readUint8());

    return string;
}

void SaveFile::write(Vector2 value)
{
    writeInt32(value.x);
    writeInt32(value.y);
}

Vector2 SaveFile::readVector2() const
{
    auto x = readInt32();
    auto y = readInt32();
    return Vector2(x, y);
}

void SaveFile::write(Vector3 value)
{
    writeInt32(value.x);
    writeInt32(value.y);
    writeInt32(value.z);
}

Vector3 SaveFile::readVector3() const
{
    auto x = readInt32();
    auto y = readInt32();
    auto z = readInt32();
    return Vector3(x, y, z);
}

SaveFile SaveFile::copyToMemory()
{
    auto offset = getOffset();
    seek(0);
    auto size = getSize();
    std::vector<char> buffer(size);
    size_t bytesRead = SDL_RWread(file.get(), buffer.data(), 1, size);

    if (bytesRead == 0)
        throw std::runtime_error(SDL_GetError());

    if (bytesRead != size)
        throw std::runtime_error("SDL_RWread didn't read the requested number of bytes");

    seek(offset);
    return SaveFile(std::move(buffer));
}
