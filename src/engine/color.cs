using static System.Math;

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
    const uint max = (1 << bitsPerChannel) - 1;
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

    Color32(uint red, uint green, uint blue, uint alpha = max)
    {
        this.value = createValue(red, green, blue, alpha);
    }

    public static Color32 fromInt16(uint value)
    {
        return new Color32(
            (value >> 12 & 15) * max / 15,
            (value >>  8 & 15) * max / 15,
            (value >>  4 & 15) * max / 15,
            (value >>  0 & 15) * max / 15
        );
    }

    uint get(Channel channel) { return value >> bit[(int) channel] & max; }
    void set(Channel channel, uint n) { value = (~(max << bit[(int) channel]) & value) | n << bit[(int) channel]; }
    void edit(Channel channel, uint n) { set(channel, Clamp(get(channel) + n, 0, max)); }

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
        set(Channel.Red, Max(getRed(), other.getRed()));
        set(Channel.Green, Max(getGreen(), other.getGreen()));
        set(Channel.Blue, Max(getBlue(), other.getBlue()));
    }

    public static Color32 operator*(Color32 color, double mod)
    {
        int delta = (int) ((modulateTemperature ? 1 : 0) * Sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) * temperatureCoefficient);
        var red = Clamp((uint) (color.getRed() * mod + 0.5 - delta), 0, max);
        var green = Clamp((uint) (color.getGreen() * mod + 0.5), 0, max);
        var blue = Clamp((uint) (color.getBlue() * mod + 0.5 + delta), 0, max);
        return new Color32(red, green, blue, color.getAlpha());
    }

    public static explicit operator bool(Color32 color) { return color.value != 0; }

    uint getMask(Channel channel) { return max << bit[(int) channel]; }

    static uint createValue(uint red, uint green, uint blue, uint alpha = max)
    {
        return red << bit[(int) Channel.Red] |
               green << bit[(int) Channel.Green] |
               blue << bit[(int) Channel.Blue] |
               alpha << bit[(int) Channel.Alpha];
    }
}
