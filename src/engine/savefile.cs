class SaveFile
{
    List<char> buffer;
    std::unique_ptr<SDL_RWops, void (*)(SDL_RWops)> file;


    template<typename T>
    void write(const List<T>& vector)
    {
        writeInt32(int(vector.size()));

        for (var element : vector)
            write(element);
    }

    template<typename T>
    void read(List<T>& vector)
    {
        var size = readInt32();
        assert(vector.empty());
        vector.reserve(size_t(size));

        for (int i = 0; i < size; ++i)
            vector.push_back(read<T>());
    }

    template<>
    double read<double>()
    {
        return readDouble();
    }
    static void closeFile(SDL_RWops file)
    {
        SDL_RWclose(file);
    }

    SaveFile(string filePath, bool writable)
    :   file(SDL_RWFromFile(filePath.c_str(), writable ? "wb" : "rb"), closeFile)
    {
        if (!file)
            throw std::runtime_error(SDL_GetError());
    }

    SaveFile(List<char> buffer)
    :   buffer(std::move(buffer)),
        file(SDL_RWFromMem(this.buffer.data(), this.buffer.size()), closeFile)
    {
        if (!file)
            throw std::runtime_error(SDL_GetError());
    }

    ulong getSize()
    {
        var size = SDL_RWsize(file.get());

        if (size < 0)
            throw std::runtime_error(SDL_GetError());

        return ulong(size);
    }

    long getOffset()
    {
        return SDL_RWtell(file.get());
    }

    void seek(long offset)
    {
        SDL_RWseek(file.get(), offset, RW_SEEK_SET);
    }

    void writeInt8(byte value)
    {
        SDL_WriteU8(file.get(), value);
    }

    byte readUint8()
    {
        return SDL_ReadU8(file.get());
    }

    void writeInt16(ushort value)
    {
        SDL_WriteLE16(file.get(), value);
    }

    ushort readUint16()
    {
        return SDL_ReadLE16(file.get());
    }

    void writeInt32(uint value)
    {
        SDL_WriteLE32(file.get(), value);
    }

    uint readUint32()
    {
        return SDL_ReadLE32(file.get());
    }

    void writeInt64(ulong value)
    {
        SDL_WriteLE64(file.get(), value);
    }

    ulong readUint64()
    {
        return SDL_ReadLE64(file.get());
    }

    void write(bool value)
    {
        writeInt8(byte(value));
    }

    bool readBool()
    {
        return bool(readUint8());
    }

    void write(double value)
    {
        ulong integer;
        static_assert(sizeof(value) == sizeof(integer), "");
        std::memcpy(integer, value, sizeof(integer));
        writeInt64(integer);
    }

    double readDouble()
    {
        var integer = readUint64();
        double result;
        static_assert(sizeof(integer) == sizeof(result), "");
        std::memcpy(result, integer, sizeof(result));
        return result;
    }

    void write(string value)
    {
        writeInt16(ushort(value.size()));

        for (var ch : value)
            writeInt8(byte(ch));
    }

    string readString()
    {
        var size = readUint16();
        string string;
        string.reserve(size);

        for (int i = 0; i < size; ++i)
            string += char(readUint8());

        return string;
    }

    void write(Vector2 value)
    {
        writeInt32(value.x);
        writeInt32(value.y);
    }

    Vector2 readVector2()
    {
        var x = readInt32();
        var y = readInt32();
        return Vector2(x, y);
    }

    void write(Vector3 value)
    {
        writeInt32(value.x);
        writeInt32(value.y);
        writeInt32(value.z);
    }

    Vector3 readVector3()
    {
        var x = readInt32();
        var y = readInt32();
        var z = readInt32();
        return Vector3(x, y, z);
    }

    SaveFile copyToMemory()
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

    void writeInt8(sbyte value) { writeInt8(byte(value)); }
    void writeInt16(short value) { writeInt16(ushort(value)); }
    void writeInt32(int value) { writeInt32(uint(value)); }
    void writeInt64(long value) { writeInt64(ulong(value)); }
    void write(string value) { write(string(value)); }

    sbyte readInt8() { return sbyte(readUint8()); }
    short readInt16() { return short(readUint16()); }
    int readInt32() { return int(readUint32()); }
    long readInt64() { return long(readUint64()); }

    template<typename T>
    void write(const std::unique_ptr<T>& value) { value.save(*this); }
    template<typename T>
    void write(T value) { value.save(*this); }
    template<typename T>
    T read() { return T::load(*this); }
}

