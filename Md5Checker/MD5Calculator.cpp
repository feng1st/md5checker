#include "stdafx.h"
#include "MD5Calculator.h"

// Constants for MD5Transform routine.
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & ~(z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | ~(z)))

// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
#define FF(a, b, c, d, x, s, ac) {(a) += F((b), (c), (d)) + (x) + (ac); (a) = ROTATE_LEFT((a), (s)); (a) += (b);}
#define GG(a, b, c, d, x, s, ac) {(a) += G((b), (c), (d)) + (x) + (ac); (a) = ROTATE_LEFT((a), (s)); (a) += (b);}
#define HH(a, b, c, d, x, s, ac) {(a) += H((b), (c), (d)) + (x) + (ac); (a) = ROTATE_LEFT((a), (s)); (a) += (b);}
#define II(a, b, c, d, x, s, ac) {(a) += I((b), (c), (d)) + (x) + (ac); (a) = ROTATE_LEFT((a), (s)); (a) += (b);}

static BYTE PADDING[64] = {0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// MD5 initialization. Begins an MD5 operation, writing a new context.
void CMD5Calculator::Init()
{
    m_count[0] = m_count[1] = 0;
    // Load magic initialization constants.
    m_state[0] = 0x67452301;
    m_state[1] = 0xEFCDAB89;
    m_state[2] = 0x98BADCFE;
    m_state[3] = 0x10325476;
}

// MD5 block update operation. Continues an MD5 message-digest
// operation, processing another message block, and updating the
// context.
void CMD5Calculator::Update(const LPBYTE input, UINT inputLen)
{
    UINT i, index, partLen;

    // Compute number of bytes mod 64
    index = (UINT)((m_count[0] >> 3) & 0x3F);

    // Update number of bits
    if ((m_count[0] += ((ULONG)inputLen << 3)) < ((ULONG)inputLen << 3))
        m_count[1]++;
    m_count[1] += ((ULONG)inputLen >> 29);

    partLen = 64 - index;

    // Transform as many times as possible.
    if (inputLen >= partLen)
    {
        memcpy((LPVOID)&m_buffer[index], (LPCVOID)input, partLen);
        Transform(m_buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
            Transform(&input[i]);

        index = 0;
    }
    else
        i = 0;

    // Buffer remaining input
    memcpy((LPVOID)&m_buffer[index], (LPCVOID)&input[i], inputLen - i);
}

// MD5 finalization. Ends an MD5 message-digest operation, writing the
// the message digest and zeroizing the context.
LPBYTE CMD5Calculator::Finalize(BYTE digest[16])
{
    BYTE bits[8];
    UINT index, padLen;

    // Save number of bits
    memcpy((LPVOID)bits, (LPCVOID)m_count, 8);

    // Pad out to 56 mod 64.
    index = (UINT)((m_count[0] >> 3) & 0x3F);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    Update(PADDING, padLen);

    // Append length (before padding)
    Update(bits, 8);

    // Store state in digest
    memcpy((LPVOID)digest, (LPCVOID)m_state, 16);

    return digest;
}

// MD5 basic transformation. Transforms state based on block.
void CMD5Calculator::Transform(const BYTE block[64])
{
    ULONG a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3], x[16];

    memcpy((LPVOID)x, (LPCVOID)block, 64);

    // Round 1
    FF(a, b, c, d, x[0], S11, 0xD76AA478);  // 1
    FF(d, a, b, c, x[1], S12, 0xE8C7B756);  // 2
    FF(c, d, a, b, x[2], S13, 0x242070DB);  // 3
    FF(b, c, d, a, x[3], S14, 0xC1BDCEEE);  // 4
    FF(a, b, c, d, x[4], S11, 0xF57C0FAF);  // 5
    FF(d, a, b, c, x[5], S12, 0x4787C62A);  // 6
    FF(c, d, a, b, x[6], S13, 0xA8304613);  // 7
    FF(b, c, d, a, x[7], S14, 0xFD469501);  // 8
    FF(a, b, c, d, x[8], S11, 0x698098D8);  // 9
    FF(d, a, b, c, x[9], S12, 0x8B44F7AF);  // 10
    FF(c, d, a, b, x[10], S13, 0xFFFF5BB1); // 11
    FF(b, c, d, a, x[11], S14, 0x895CD7BE); // 12
    FF(a, b, c, d, x[12], S11, 0x6B901122); // 13
    FF(d, a, b, c, x[13], S12, 0xFD987193); // 14
    FF(c, d, a, b, x[14], S13, 0xA679438E); // 15
    FF(b, c, d, a, x[15], S14, 0x49B40821); // 16

    // Round 2
    GG(a, b, c, d, x[1], S21, 0xF61E2562);  // 17
    GG(d, a, b, c, x[6], S22, 0xC040B340);  // 18
    GG(c, d, a, b, x[11], S23, 0x265E5A51); // 19
    GG(b, c, d, a, x[0], S24, 0xE9B6C7AA);  // 20
    GG(a, b, c, d, x[5], S21, 0xD62F105D);  // 21
    GG(d, a, b, c, x[10], S22, 0x2441453);  // 22
    GG(c, d, a, b, x[15], S23, 0xD8A1E681); // 23
    GG(b, c, d, a, x[4], S24, 0xE7D3FBC8);  // 24
    GG(a, b, c, d, x[9], S21, 0x21E1CDE6);  // 25
    GG(d, a, b, c, x[14], S22, 0xC33707D6); // 26
    GG(c, d, a, b, x[3], S23, 0xF4D50D87);  // 27
    GG(b, c, d, a, x[8], S24, 0x455A14ED);  // 28
    GG(a, b, c, d, x[13], S21, 0xA9E3E905); // 29
    GG(d, a, b, c, x[2], S22, 0xFCEFA3F8);  // 30
    GG(c, d, a, b, x[7], S23, 0x676F02D9);  // 31
    GG(b, c, d, a, x[12], S24, 0x8D2A4C8A); // 32

    // Round 3
    HH(a, b, c, d, x[5], S31, 0xFFFA3942);  // 33
    HH(d, a, b, c, x[8], S32, 0x8771F681);  // 34
    HH(c, d, a, b, x[11], S33, 0x6D9D6122); // 35
    HH(b, c, d, a, x[14], S34, 0xFDE5380C); // 36
    HH(a, b, c, d, x[1], S31, 0xA4BEEA44);  // 37
    HH(d, a, b, c, x[4], S32, 0x4BDECFA9);  // 38
    HH(c, d, a, b, x[7], S33, 0xF6BB4B60);  // 39
    HH(b, c, d, a, x[10], S34, 0xBEBFBC70); // 40
    HH(a, b, c, d, x[13], S31, 0x289B7EC6); // 41
    HH(d, a, b, c, x[0], S32, 0xEAA127FA);  // 42
    HH(c, d, a, b, x[3], S33, 0xD4EF3085);  // 43
    HH(b, c, d, a, x[6], S34, 0x4881D05);   // 44
    HH(a, b, c, d, x[9], S31, 0xD9D4D039);  // 45
    HH(d, a, b, c, x[12], S32, 0xE6DB99E5); // 46
    HH(c, d, a, b, x[15], S33, 0x1FA27CF8); // 47
    HH(b, c, d, a, x[2], S34, 0xC4AC5665);  // 48

    // Round 4
    II(a, b, c, d, x[0], S41, 0xF4292244);  // 49
    II(d, a, b, c, x[7], S42, 0x432AFF97);  // 50
    II(c, d, a, b, x[14], S43, 0xAB9423A7); // 51
    II(b, c, d, a, x[5], S44, 0xFC93A039);  // 52
    II(a, b, c, d, x[12], S41, 0x655B59C3); // 53
    II(d, a, b, c, x[3], S42, 0x8F0CCC92);  // 54
    II(c, d, a, b, x[10], S43, 0xFFEFF47D); // 55
    II(b, c, d, a, x[1], S44, 0x85845DD1);  // 56
    II(a, b, c, d, x[8], S41, 0x6FA87E4F);  // 57
    II(d, a, b, c, x[15], S42, 0xFE2CE6E0); // 58
    II(c, d, a, b, x[6], S43, 0xA3014314);  // 59
    II(b, c, d, a, x[13], S44, 0x4E0811A1); // 60
    II(a, b, c, d, x[4], S41, 0xF7537E82);  // 61
    II(d, a, b, c, x[11], S42, 0xBD3AF235); // 62
    II(c, d, a, b, x[2], S43, 0x2AD7D2BB);  // 63
    II(b, c, d, a, x[9], S44, 0xEB86D391);  // 64

    m_state[0] += a;
    m_state[1] += b;
    m_state[2] += c;
    m_state[3] += d;
}
