struct Color32
{
    uint value;

    enum Channel
    {
        Red,
        Green,
        Blue,
        Alpha
    }

    const int channelCount = 4;
    const int depth = sizeof(uint) * 8;
    const int bitsPerChannel = depth / channelCount;
    const int max = (1 << bitsPerChannel) - 1;
    static readonly int[] bit = { 3 * bitsPerChannel, 2 * bitsPerChannel, 1 * bitsPerChannel, 0 * bitsPerChannel };
    const int temperatureCoefficient = int(0.25 * max);
    static readonly Color<uint> white = new Color<uint>(max, max, max);
    static readonly Color<uint> black = new Color<uint>(0, 0, 0);
    static readonly Color<uint> none = new Color<uint>(0);
    static bool modulateTemperature = true;

    Color() {}
    Color(uint value) : value(value) {}
    Color(int red, int green, int blue, int alpha = max) : value(createValue(red, green, blue, alpha)) {}

    void fromColor16(ushort value)
    {

    }

    int get(Channel channel) { return value >> bit[channel] & max; }
    void set(Channel channel, int n) { value = (~(max << bit[channel]) & value) | n << bit[channel]; }
    void edit(Channel channel, int n) { set(channel, limit(get(channel) + n, 0, max)); }

    int getRed() { return get(Channel.Red); }
    int getGreen() { return get(Channel.Green); }
    int getBlue() { return get(Channel.Blue); }
    int getAlpha() { return get(Channel.Alpha); }

    double getLuminance()
    {
        return (0.299 * getRed() + 0.587 * getGreen() + 0.114 * getBlue()) / max;
    }

    void lighten(Color32 other)
    {
        set(Channel.Red, std::max(getRed(), other.getRed()));
        set(Channel.Green, std::max(getGreen(), other.getGreen()));
        set(Channel.Blue, std::max(getBlue(), other.getBlue()));
    }

    Color operator=(double mod)
    {
        int delta = int(modulateTemperature * sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) *
                        temperatureCoefficient);
        int red = int(limit(int(getRed() * mod + 0.5 - delta), 0, int(max)));
        int green = int(limit(int(getGreen() * mod + 0.5), 0, int(max)));
        int blue = int(limit(int(getBlue() * mod + 0.5 + delta), 0, int(max)));
        value = createValue(red, green, blue, getAlpha());
        return this;
    }

    Color operator*(double mod) { return Color(this) *= mod; }

    explicit operator bool() { return value != 0; }

    template<typename U>
    operator Color<U>()
    {
        return Color<U>(getRed() * Color<U>::max / max, getGreen() * Color<U>::max / max,
                        getBlue() * Color<U>::max / max, getAlpha() * Color<U>::max / max);
    }

    const var getMask(Channel channel) { return max << bit[channel]; }

private:
    static uint createValue(int red, int green, int blue, int alpha = max)
    {
        return red << bit[Channel.Red] | green << bit[Channel.Green] | blue << bit[Channel.Blue] | alpha << bit[Channel.Alpha];
    }
}

