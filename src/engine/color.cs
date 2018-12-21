template<typename T>
struct Color;

using Color16 = Color<uint16_t>;
using Color32 = Color<uint32_t>;

template<typename T>
struct Color
{
    enum Channel { Red, Green, Blue, Alpha }

    static const int channelCount = 4;
    static const int depth = sizeof(T) * CHAR_BIT;
    static const int bitsPerChannel = depth / channelCount;
    static const int max = (1 << bitsPerChannel) - 1;
    static const int bit[channelCount];
    static const int temperatureCoefficient = int(0.25 * max);
    static const Color white;
    static const Color black;
    static const Color none;
    static bool modulateTemperature;

    T value;

    Color() = default;
    explicit Color(T value) : value(value) {}
    Color(int red, int green, int blue, int alpha = max) : value(createValue(red, green, blue, alpha)) {}

    int get(Channel channel) const { return value >> bit[channel] & max; }
    void set(Channel channel, int n) { value = (~(max << bit[channel]) & value) | n << bit[channel]; }
    void edit(Channel channel, int n) { set(channel, limit(get(channel) + n, 0, max)); }

    int getRed() const { return get(Red); }
    int getGreen() const { return get(Green); }
    int getBlue() const { return get(Blue); }
    int getAlpha() const { return get(Alpha); }

    double getLuminance() const
    {
        return (0.299 * getRed() + 0.587 * getGreen() + 0.114 * getBlue()) / max;
    }

    void lighten(Color32 other)
    {
        set(Red, std::max(getRed(), other.getRed()));
        set(Green, std::max(getGreen(), other.getGreen()));
        set(Blue, std::max(getBlue(), other.getBlue()));
    }

    Color operator=(double mod)
    {
        int delta = int(modulateTemperature * sign(mod - 1.0) * ((2.0 - mod) * mod - 1.0) *
                        temperatureCoefficient);
        int red = int(limit(int(getRed() * mod + 0.5 - delta), 0, int(max)));
        int green = int(limit(int(getGreen() * mod + 0.5), 0, int(max)));
        int blue = int(limit(int(getBlue() * mod + 0.5 + delta), 0, int(max)));
        value = createValue(red, green, blue, getAlpha());
        return *this;
    }

    Color operator(double mod) const { return Color(*this) *= mod; }

    explicit operator bool() const { return value != 0; }

    template<typename U>
    operator Color<U>() const
    {
        return Color<U>(getRed() * Color<U>::max / max, getGreen() * Color<U>::max / max,
                        getBlue() * Color<U>::max / max, getAlpha() * Color<U>::max / max);
    }

    static constexpr var getMask(Channel channel) { return max << bit[channel]; }

private:
    static inline T createValue(int red, int green, int blue, int alpha = max)
    {
        return static_cast<T>(red << bit[Red] | green << bit[Green] | blue << bit[Blue] | alpha << bit[Alpha]);
    }
}

template<typename T>
const int Color<T>::bit[] =
{
    3 * bitsPerChannel, 2 * bitsPerChannel, 1 * bitsPerChannel, 0 * bitsPerChannel
}

template<typename T>
bool Color<T>::modulateTemperature = true;

template<typename T>
const Color<T> Color<T>::white(max, max, max);

template<typename T>
const Color<T> Color<T>::black(0, 0, 0);

template<typename T>
const Color<T> Color<T>::none(0);
