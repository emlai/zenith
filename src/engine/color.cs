struct Color32
{
    public uint value;

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
    const int temperatureCoefficient = (int) (0.25 * max);
    static readonly Color32 white = new Color32(max, max, max);
    static readonly Color32 black = new Color32(0, 0, 0);
    static readonly Color32 none = new Color32(0);
    static bool modulateTemperature = true;

    Color32(uint value)
    {
        this.value = value;
    }

    Color32(int red, int green, int blue, int alpha = max)
    {
        this.value = createValue(red, green, blue, alpha);
    }

    public static Color32 fromColor16(ushort value)
    {
        const int color16Max = 15;
        return new Color32(
            (value >> bit[(int) Channel.Red] & color16Max) * max / color16Max,
            (value >> bit[(int) Channel.Green] & color16Max) * max / color16Max,
            (value >> bit[(int) Channel.Blue] & color16Max) * max / color16Max,
            (value >> bit[(int) Channel.Alpha] & color16Max) * max / color16Max
        );
    }

    uint get(Channel channel) { return value >> bit[(int) channel] & max; }
    void set(Channel channel, int n) { value = (~(max << bit[(int) channel]) & value) | n << bit[(int) channel]; }
    void edit(Channel channel, int n) { set(channel, limit(get(channel) + n, 0, max)); }

    uint getRed() { return get(Channel.Red); }
    uint getGreen() { return get(Channel.Green); }
    uint getBlue() { return get(Channel.Blue); }
    uint getAlpha() { return get(Channel.Alpha); }

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

    Color32 operator*=(double mod)
    {
        int delta = int(modulateTemperature * sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) *
                        temperatureCoefficient);
        int red = int(limit(int(getRed() * mod + 0.5 - delta), 0, int(max)));
        int green = int(limit(int(getGreen() * mod + 0.5), 0, int(max)));
        int blue = int(limit(int(getBlue() * mod + 0.5 + delta), 0, int(max)));
        value = createValue(red, green, blue, getAlpha());
        return this;
    }

    Color32 operator*(double mod) { return Color32(this) *= mod; }

    explicit operator bool() { return value != 0; }

    int getMask(Channel channel) { return max << bit[channel]; }

    static uint createValue(int red, int green, int blue, int alpha = max)
    {
        return red << bit[Channel.Red] | green << bit[Channel.Green] | blue << bit[Channel.Blue] | alpha << bit[Channel.Alpha];
    }
}

