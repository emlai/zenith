using Vector2 = Vector2Base<int>;

const double pi = 3.14159265358979324;

template<typename T>
T sign(T value)
{
    return (value > 0) - (value < 0);
}

template<typename T>
T limit(T value, T min, T max)
{
    return std::min(std::max(value, min), max);
}

/* Pseudo-random number generation */

struct Xorshift1024Star
{
    ulong operator()();
    ulong state[16];
    int index = 0;
}

struct Xorshift64Star
{
    ulong operator()();
    ulong state;
}

class RNG
{
    using Generator = Xorshift1024Star;
    using result_type = ulong;

    RNG(Generator algorithm) : algorithm(algorithm) {}
    void seed();
    void seed(RNG::result_type);
    var getSeed() { return currentSeed; }
    result_type operator()() { return algorithm(); }
    const result_type max() { return std::numeric_limits<result_type>::max(); }
    const result_type min() { return std::numeric_limits<result_type>::min(); }

private:
    Generator algorithm;
    result_type currentSeed;
}

extern RNG rng;

template<typename T> T randInt(T max = std::numeric_limits<T>::max());
template<typename T> T randInt(T min, T max);
template<> RNG::result_type randInt(RNG::result_type max);
template<> RNG::result_type randInt(RNG::result_type min, RNG::result_type max);
double randFloat(double max = 1.0);
double randFloat(double min, double max);
double randNormal(double stdDev = 1.0);

template<typename T>
T randInt(T max)
{
    return T(randInt(RNG::result_type(max)));
}

template<typename T>
T randInt(T min, T max)
{
    return T(randInt(RNG::result_type(min), RNG::result_type(max)));
}

template<typename IndexableContainer>
var randomElement(IndexableContainer container)
{
    assert(!container.empty());
    return container[randInt(container.size() - 1)];
}

/// Calls `process` with each point on a line from `source` to `target` as determined by Bresenham's
/// line algorithm. Stops processing and returns false if `process` returns false. Otherwise returns
/// true after processing all points.
bool raycastIntegerBresenham(Vector2 source, Vector2 target, const std::function<bool(Vector2)>& process);
const Vector2 directionVectors[] =
{
    {0, 0}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
}

RNG rng = RNG(Xorshift1024Star());

void RNG::seed()
{
    std::random_device randomDevice;
    seed((ulong(randomDevice()) << 32) | randomDevice());
}

void RNG::seed(RNG::result_type seed)
{
    currentSeed = seed;

    Xorshift64Star seedGenerator{seed}
    std::generate(std::begin(algorithm.state), std::end(algorithm.state), seedGenerator);
}

ulong Xorshift1024Star::operator()()
{
    var s0 = state[index];
    var s1 = state[index = (index + 1) & 15];
    s1 ^= s1 << 31;
    s1 ^= s1 >> 11;
    s0 ^= s0 >> 30;
    return (state[index] = s0 ^ s1) * 1181783497276652981ULL;
}

ulong Xorshift64Star::operator()()
{
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 2685821657736338717ULL;
}

template<>
RNG::result_type randInt(RNG::result_type max)
{
    return std::uniform_int_distribution<RNG::result_type>(0, max)(rng);
}

template<>
RNG::result_type randInt(RNG::result_type min, RNG::result_type max)
{
    return std::uniform_int_distribution<RNG::result_type>(min, max)(rng);
}

double randFloat(double max)
{
    return std::uniform_real_distribution<double>(0, max)(rng);
}

double randFloat(double min, double max)
{
    return std::uniform_real_distribution<double>(min, max)(rng);
}

double randNormal(double stdDev)
{
    return std::normal_distribution<double>(0.0, stdDev)(rng);
}

bool raycastIntegerBresenham(Vector2 source, Vector2 target, const std::function<bool(Vector2)>& process)
{
    const Vector2 delta = target - source;
    const Vector2 sign = ::sign(delta);
    const Vector2 abs = ::abs(delta) * 2; // x2 to avoid rounding errors
    Vector2 current = source;
    int slope = 0;

    if (abs.x > abs.y)
    {
        while (current.x != target.x)
        {
            if (slope > 0)
            {
                current.y += sign.y;
                slope -= abs.x;
            }

            current.x += sign.x;
            slope += abs.y;

            if (!process(current))
                return false;
        }
    }
    else if (abs.x < abs.y)
    {
        while (current.y != target.y)
        {
            if (slope > 0)
            {
                current.x += sign.x;
                slope -= abs.y;
            }

            current.y += sign.y;
            slope += abs.x;

            if (!process(current))
                return false;
        }
    }
    else
    {
        while (current.x != target.x)
        {
            current += sign;

            if (!process(current))
                return false;
        }
    }

    return true;
}
