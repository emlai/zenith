struct SDL_RWops;

class SaveFile
{
public:
    SaveFile(string filePath, bool writable);
    ulong getSize();
    long getOffset();
    void seek(long offset);
    SaveFile copyToMemory();

    void writeInt8(sbyte value) { writeInt8(byte(value)); }
    void writeInt8(byte value);
    void writeInt16(short value) { writeInt16(ushort(value)); }
    void writeInt16(ushort value);
    void writeInt32(int value) { writeInt32(uint(value)); }
    void writeInt32(uint value);
    void writeInt64(long value) { writeInt64(ulong(value)); }
    void writeInt64(ulong value);
    void write(bool value);
    void write(double value);
    void write(Vector2 value);
    void write(Vector3 value);
    void write(string value);
    void write(string value) { write(string(value)); }
    template<typename T>
    void write(const List<T>& vector);

    sbyte readInt8() { return sbyte(readUint8()); }
    byte readUint8();
    short readInt16() { return short(readUint16()); }
    ushort readUint16();
    int readInt32() { return int(readUint32()); }
    uint readUint32();
    long readInt64() { return long(readUint64()); }
    ulong readUint64();
    bool readBool();
    double readDouble();
    string readString();
    Vector2 readVector2();
    Vector3 readVector3();
    template<typename T>
    void read(List<T>& vector);

private:
    SaveFile(List<char> buffer);
    template<typename T>
    void write(const std::unique_ptr<T>& value) { value->save(*this); }
    template<typename T>
    void write(T value) { value.save(*this); }
    template<typename T>
    T read() { return T::load(*this); }

    List<char> buffer;
    std::unique_ptr<SDL_RWops, void (*)(SDL_RWops)> file;
}

template<typename T>
void SaveFile::write(const List<T>& vector)
{
    writeInt32(int(vector.size()));

    for (var element : vector)
        write(element);
}

template<typename T>
void SaveFile::read(List<T>& vector)
{
    var size = readInt32();
    assert(vector.empty());
    vector.reserve(size_t(size));

    for (int i = 0; i < size; ++i)
        vector.push_back(read<T>());
}

template<>
inline double SaveFile::read<double>()
{
    return readDouble();
}
static void closeFile(SDL_RWops file)
{
    SDL_RWclose(file);
}

SaveFile::SaveFile(string filePath, bool writable)
:   file(SDL_RWFromFile(filePath.to_string().c_str(), writable ? "wb" : "rb"), closeFile)
{
    if (!file)
        throw std::runtime_error(SDL_GetError());
}

SaveFile::SaveFile(List<char> buffer)
:   buffer(std::move(buffer)),
    file(SDL_RWFromMem(this->buffer.data(), this->buffer.size()), closeFile)
{
    if (!file)
        throw std::runtime_error(SDL_GetError());
}

ulong SaveFile::getSize()
{
    var size = SDL_RWsize(file.get());

    if (size < 0)
        throw std::runtime_error(SDL_GetError());

    return ulong(size);
}

long SaveFile::getOffset()
{
    return SDL_RWtell(file.get());
}

void SaveFile::seek(long offset)
{
    SDL_RWseek(file.get(), offset, RW_SEEK_SET);
}

void SaveFile::writeInt8(byte value)
{
    SDL_WriteU8(file.get(), value);
}

byte SaveFile::readUint8()
{
    return SDL_ReadU8(file.get());
}

void SaveFile::writeInt16(ushort value)
{
    SDL_WriteLE16(file.get(), value);
}

ushort SaveFile::readUint16()
{
    return SDL_ReadLE16(file.get());
}

void SaveFile::writeInt32(uint value)
{
    SDL_WriteLE32(file.get(), value);
}

uint SaveFile::readUint32()
{
    return SDL_ReadLE32(file.get());
}

void SaveFile::writeInt64(ulong value)
{
    SDL_WriteLE64(file.get(), value);
}

ulong SaveFile::readUint64()
{
    return SDL_ReadLE64(file.get());
}

void SaveFile::write(bool value)
{
    writeInt8(byte(value));
}

bool SaveFile::readBool()
{
    return bool(readUint8());
}

void SaveFile::write(double value)
{
    ulong integer;
    static_assert(sizeof(value) == sizeof(integer), "");
    std::memcpy(integer, value, sizeof(integer));
    writeInt64(integer);
}

double SaveFile::readDouble()
{
    var integer = readUint64();
    double result;
    static_assert(sizeof(integer) == sizeof(result), "");
    std::memcpy(result, integer, sizeof(result));
    return result;
}

void SaveFile::write(string value)
{
    writeInt16(ushort(value.size()));

    for (var ch : value)
        writeInt8(byte(ch));
}

string SaveFile::readString()
{
    var size = readUint16();
    string string;
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

Vector2 SaveFile::readVector2()
{
    var x = readInt32();
    var y = readInt32();
    return Vector2(x, y);
}

void SaveFile::write(Vector3 value)
{
    writeInt32(value.x);
    writeInt32(value.y);
    writeInt32(value.z);
}

Vector3 SaveFile::readVector3()
{
    var x = readInt32();
    var y = readInt32();
    var z = readInt32();
    return Vector3(x, y, z);
}

SaveFile SaveFile::copyToMemory()
{
    var offset = getOffset();
    seek(0);
    var size = getSize();
    List<char> buffer(size);
    size_t bytesRead = SDL_RWread(file.get(), buffer.data(), 1, size);

    if (bytesRead == 0)
        throw std::runtime_error(SDL_GetError());

    if (bytesRead != size)
        throw std::runtime_error("SDL_RWread didn't read the requested number of bytes");

    seek(offset);
    return SaveFile(std::move(buffer));
}
