// Demonstration code for Zobrist Hash using the Mersenne Twister
// Compiles under Visual Studio .NET 2003 under Windows and gcc under Linux.
// By Toby Jones (tjones@humanhead.com)

#include <iostream>
#include <cassert>

// Specialized templates to compute powers of two at compile time
// (template metaprogramming)
template<int N>
struct Pow2
{
    enum { value = 2 * Pow2<N - 1>::value };
};

template<>
struct Pow2<0>
{
    enum { value = 1 };
};

template<int N>
struct Pow2Minus1
{
    enum { value = Pow2<N - 1>::value - 1 + Pow2<N - 1>::value };
};

// This is defined in C99, but not C++ yet (June 2003)
typedef unsigned long uint32_t;
#if defined(_WIN32)
typedef unsigned __int64 uint64_t;
#elif defined (__GNUC__)
typedef unsigned long long uint64_t;
#endif

// Parameters for MT19937
// See Matsumoto for definition of parameters, and
// alternate parameters for different k-distributions.
static const int      MT_W = 32;            // word size
static const int      MT_N = 624;           // degree of recursion
static const int      MT_M = 397;           // middle term
static const int      MT_R = 31;            // separation point of one word
static const uint32_t MT_A = 0x9908b0df;    // vector parameter a (matrix A)
static const int      MT_U = 11;            // integer parameter u
static const int      MT_S = 7;             // integer parameter s
static const uint32_t MT_B = 0x9d2c5680;    // vector parameter b
static const int      MT_T = 15;            // integer parameter t
static const uint32_t MT_C = 0xefc60000;    // vector parameter c
static const int      MT_L = 18;            // integer parameter l

// Autogenerate the masks based on the R parameter. All of the
// proposed MT parameters have a 32-bit word size, so I assume that.
static const uint32_t MT_LLMASK = Pow2Minus1<MT_R>::value;
static const uint32_t MT_UMASK  = 0xffffffff - Pow2Minus1<MT_R>::value;

// All values for the array are fine initial seed choices, except for
// an array of all zeros.
static uint32_t s_aMT[MT_N] = {
       2,    3,    5,    7,   11,   13,   17,   19,   23,   29,
      31,   37,   41,   43,   47,   53,   59,   61,   67,   71,
      73,   79,   83,   89,   97,  101,  103,  107,  109,  113,
     127,  131,  137,  139,  149,  151,  157,  163,  167,  173,
     179,  181,  191,  193,  197,  199,  211,  223,  227,  229,
     233,  239,  241,  251,  257,  263,  269,  271,  277,  281,
     283,  293,  307,  311,  313,  317,  331,  337,  347,  349,
     353,  359,  367,  373,  379,  383,  389,  397,  401,  409,
     419,  421,  431,  433,  439,  443,  449,  457,  461,  463,
     467,  479,  487,  491,  499,  503,  509,  521,  523,  541,
     547,  557,  563,  569,  571,  577,  587,  593,  599,  601,
     607,  613,  617,  619,  631,  641,  643,  647,  653,  659,
     661,  673,  677,  683,  691,  701,  709,  719,  727,  733,
     739,  743,  751,  757,  761,  769,  773,  787,  797,  809,
     811,  821,  823,  827,  829,  839,  853,  857,  859,  863,
     877,  881,  883,  887,  907,  911,  919,  929,  937,  941,
     947,  953,  967,  971,  977,  983,  991,  997, 1009, 1013,
    1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069,
    1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151,
    1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223,
    1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
    1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
    1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451,
    1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
    1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583,
    1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657,
    1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733,
    1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811,
    1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889,
    1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987,
    1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053,
    2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129,
    2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213,
    2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287,
    2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357,
    2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423,
    2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531,
    2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617,
    2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687,
    2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741,
    2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819,
    2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903,
    2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999,
    3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079,
    3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181,
    3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257,
    3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331,
    3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413,
    3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511,
    3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571,
    3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643,
    3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727,
    3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821,
    3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907,
    3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989,
    4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057,
    4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139,
    4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231,
    4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297,
    4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409,
    4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493,
    4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583,
    4591, 4597, 4603, 4621
};

class MersenneTwister
{
    int m_ix;

    void Regenerate();

public:
    MersenneTwister();

    uint32_t Rand();
    uint64_t Rand64();
};

void MersenneTwister::Regenerate()
{
    // NOTE: this should be unrolled in an optimized version
    // to eliminate the modulus operator.
    for(int kk = 0; kk < MT_N; kk++)
    {
        uint32_t ui = (s_aMT[kk] & MT_UMASK) | (s_aMT[(kk + 1) % MT_N] & MT_LLMASK);
        s_aMT[kk] = s_aMT[(kk + MT_M) % MT_N] ^ (ui >> 1) ^ ((ui & 0x00000001) ? MT_A : 0);
    }
}

MersenneTwister::MersenneTwister() :
    m_ix(0)
{
    // s_aMT is already initialized with the first N primes.
    Regenerate();
}

uint32_t MersenneTwister::Rand()
{
    if (m_ix == MT_N)
    {
        m_ix = 0;

        Regenerate();
    }

    uint32_t y;
    y = s_aMT[m_ix++];
    y ^= y >> MT_U;
    y ^= y << MT_S & MT_B;
    y ^= y << MT_T & MT_C;
    y ^= y >> MT_L;

    return y;
}

uint64_t MersenneTwister::Rand64()
{
    uint64_t ui64;

    ui64 = Rand();
    ui64 <<= 32;
    ui64 |= Rand();

    return ui64;
}

// I only use the number of unique pieces, not the total number of pieces.  This
// is because we want a key match on "equivilent" boards.  i.e. if we transposed
// two same-colored rooks, the board is functionally the same.
static const int BOARD_SIZE = 8 * 8;    // size of chess board
static const int NUM_PIECES = 6;        // rook, knight, bishop, king, queen, pawn
static const int NUM_COLORS = 2;        // white, black

// Use a constant instead of transposing the key depending on the side to play.
// This is more efficient, since I can more easily undo operations (so I can
// incrementally update the key).
static const uint64_t BLACK_TO_MOVE = 0x8913125CFB309AFC;   // Random number to XOR into black moves

enum eChessPiece { EMPTY,
                   B_ROOK, B_KNIGHT, B_BISHOP, B_KING, B_QUEEN, B_PAWN,
                   W_ROOK, W_KNIGHT, W_BISHOP, W_KING, W_QUEEN, W_PAWN };
enum eColor { BLACK, WHITE };

class ChessBoard
{
    uint64_t m_aZobristTable[BOARD_SIZE][NUM_PIECES][NUM_COLORS];
    eChessPiece m_aBoard[BOARD_SIZE];

    void InitializeZobristTable();
    void PopulateChessBoard();

public:
    ChessBoard();
    uint64_t CalculateZobristKey(eColor sideToMove) const;
    uint64_t UpdateZobristKey(uint64_t oldKey, eChessPiece piece, int oldPos, int newPos) const;
    void MovePiece(int oldPos, int newPos);
};

void ChessBoard::InitializeZobristTable()
{
    MersenneTwister rng;

    // Use the Mersenne Twister to fill up the Zobrist random table
    for(int ii = 0; ii < BOARD_SIZE; ii++)
    {
        for(int jj = 0; jj < NUM_PIECES; jj++)
        {
            for(int kk = 0; kk < NUM_COLORS; kk++)
            {
                m_aZobristTable[ii][jj][kk] = rng.Rand64();
            }
        }
    }
}

void ChessBoard::PopulateChessBoard()
{
    // To test different hash possibilities, change this to reflect
    // the initial board state to test.
    int ii;
    for(ii = 16; ii < (BOARD_SIZE - 16); ii++)
    {
        m_aBoard[ii] = EMPTY;
    }

    // Set up pawns
    for(ii = 0; ii < 8; ii++)
    {
        m_aBoard[ii + 8] = W_PAWN;
    }
    for(ii = 0; ii < 8; ii++)
    {
        m_aBoard[ii + (BOARD_SIZE - 16)] = B_PAWN;
    }

    m_aBoard[0] = W_ROOK;
    m_aBoard[1] = W_KNIGHT;
    m_aBoard[2] = W_BISHOP;
    m_aBoard[3] = W_QUEEN;
    m_aBoard[4] = W_KING;
    m_aBoard[5] = W_BISHOP;
    m_aBoard[6] = W_KNIGHT;
    m_aBoard[7] = W_ROOK;

    m_aBoard[56] = B_ROOK;
    m_aBoard[57] = B_KNIGHT;
    m_aBoard[58] = B_BISHOP;
    m_aBoard[59] = B_QUEEN;
    m_aBoard[60] = B_KING;
    m_aBoard[61] = B_BISHOP;
    m_aBoard[62] = B_KNIGHT;
    m_aBoard[63] = B_ROOK;

    for(ii = 0; ii < BOARD_SIZE; ii++)
    {
        assert(m_aBoard[ii] >= EMPTY && m_aBoard[ii] <= W_PAWN);
    }
}

ChessBoard::ChessBoard()
{
    InitializeZobristTable();

    PopulateChessBoard();
}

uint64_t ChessBoard::CalculateZobristKey(
    eColor sideToMove) const
{
    uint64_t uZobristKey = 0;

    eChessPiece piece = EMPTY;
    eColor color      = BLACK;

    for(unsigned int ii = 0; ii < sizeof(m_aBoard) / sizeof(m_aBoard[0]); ii++)
    {
        if(EMPTY != m_aBoard[ii])
        {
            if(W_ROOK <= m_aBoard[ii])
            {
                color = WHITE;
            }
            else
            {
                color = BLACK;
            }

            piece = eChessPiece(m_aBoard[ii]);
            if(WHITE == color)
            {
                piece = eChessPiece(piece - NUM_PIECES);
            }

            uZobristKey ^= m_aZobristTable[ii][piece][color];
        }
    }

    if(BLACK == sideToMove)
    {
        uZobristKey ^= BLACK_TO_MOVE;
    }

    return uZobristKey;
}

uint64_t ChessBoard::UpdateZobristKey(
    uint64_t    oldKey,
    eChessPiece piece,
    int         oldPos,
    int         newPos) const
{
    assert(piece == m_aBoard[oldPos]);

    uint64_t newKey = oldKey;

    int color = piece < W_ROOK ? BLACK : WHITE;
    piece = piece < W_ROOK ? piece : eChessPiece(piece - NUM_PIECES);

    newKey ^= m_aZobristTable[oldPos][piece][color];    // remove piece from the key
    newKey ^= m_aZobristTable[newPos][piece][color];    // re-add piece to the key in new position

    newKey ^= BLACK_TO_MOVE;    // apply or undo previous setting

    return newKey;
}

void ChessBoard::MovePiece(int oldPos, int newPos)
{
    m_aBoard[newPos] = m_aBoard[oldPos];
    m_aBoard[oldPos] = EMPTY;
}

// Output the first 1024 generated numbers
static void TestMT()
{
    std::cout << "Outputting the first 1024 generated numbers" << std::endl;

    MersenneTwister rng;

    for(int ii = 0; ii < 1024; ii++)
    {
        std::cout << rng.Rand() << std::endl;
    }
}

// Visual C++ 6.0 does not have an unsigned 64-bit operator<<
// Visual Studio .NET 2003 and gcc do not have this issue.
static std::ostream & Output64(std::ostream & os, uint64_t ui64)
{
    os << (uint32_t)((ui64 >> 32) & 0xFFFFFFFF);
    os.unsetf(std::ios::showbase);
    os << (uint32_t)(ui64 & 0xFFFFFFFF);
    os.setf(std::ios::showbase);

    return os;
}

// Demonstrate that a full hash calculation is the same as an incremental operation
static void TestZH()
{
    ChessBoard chessBoard;

    uint64_t initialZobristKey = chessBoard.CalculateZobristKey(WHITE);

    uint64_t newIncrementalZobristKey = chessBoard.UpdateZobristKey(initialZobristKey,
                                                                    W_PAWN,
                                                                    8,
                                                                    24);

    chessBoard.MovePiece(8, 24);
    uint64_t newFullZobristKey = chessBoard.CalculateZobristKey(BLACK);

    std::cout << "Initial Zobrist Key: ";
    Output64(std::cout, initialZobristKey) << std::endl;

    std::cout << "Moving white pawn from a2 to a4..." << std::endl;

    std::cout << "New Zobrist Key (incremental): ";
    Output64(std::cout, newIncrementalZobristKey) << std::endl;
    std::cout << "New Zobrist Key (full):        ";
    Output64(std::cout, newFullZobristKey) << std::endl;

    if(newIncrementalZobristKey == newFullZobristKey)
    {
        std::cout << "Zobrist keys match." << std::endl;
    }
    else
    {
        std::cout << "Zobrist keys do _not_ match." << std::endl;
    }
}

int main()
{
    std::cout.setf(std::ios::showbase);
    std::cout.setf(std::ios::hex, std::ios::basefield);

    std::cout << "-Testing Mersenne Twister-" << std::endl;
    TestMT();

    std::cout << std::endl;

    std::cout << "-Testing Zobrist Hash-" << std::endl;
    TestZH();

    return 0;
}
