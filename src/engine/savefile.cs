struct SDL_RWops;

class SaveFile
{
public:
    SaveFile(boost::string_ref filePath, bool writable);
    uint64_t getSize() const;
    int64_t getOffset() const;
    void seek(int64_t offset);
    SaveFile copyToMemory();

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
    void write(string value) { write(boost::string_ref(value)); }
    template<typename T>
    void write(const List<T>& vector);

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
    string readString() const;
    Vector2 readVector2() const;
    Vector3 readVector3() const;
    template<typename T>
    void read(List<T>& vector) const;

private:
    SaveFile(List<char> buffer);
    template<typename T>
    void write(const std::unique_ptr<T>& value) { value->save(*this); }
    template<typename T>
    void write(T value) { value.save(*this); }
    template<typename T>
    T read() const { return T::load(*this); }

    List<char> buffer;
    std::unique_ptr<SDL_RWops, void (*)(SDL_RWops)> file;
}

template<typename T>
void SaveFile::write(const List<T>& vector)
{
    writeInt32(int32_t(vector.size()));

    for (var element : vector)
        write(element);
}

template<typename T>
void SaveFile::read(List<T>& vector) const
{
    var size = readInt32();
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
static void closeFile(SDL_RWops file)
{
    SDL_RWclose(file);
}

SaveFile::SaveFile(boost::string_ref filePath, bool writable)
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

uint64_t SaveFile::getSize() const
{
    var size = SDL_RWsize(file.get());

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
    std::memcpy(integer, value, sizeof(integer));
    writeInt64(integer);
}

double SaveFile::readDouble() const
{
    var integer = readUint64();
    double result;
    static_assert(sizeof(integer) == sizeof(result), "");
    std::memcpy(result, integer, sizeof(result));
    return result;
}

void SaveFile::write(boost::string_ref value)
{
    writeInt16(uint16_t(value.size()));

    for (var ch : value)
        writeInt8(uint8_t(ch));
}

string SaveFile::readString() const
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

Vector2 SaveFile::readVector2() const
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

Vector3 SaveFile::readVector3() const
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
