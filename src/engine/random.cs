static class Random
{
    static Dir8 dir8()
    {
        switch (randInt(7))
        {
            case 0: return East;
            case 1: return SouthEast;
            case 2: return South;
            case 3: return SouthWest;
            case 4: return West;
            case 5: return NorthWest;
            case 6: return North;
            case 7: return NorthEast;
        }

        assert(false);
    }
}
