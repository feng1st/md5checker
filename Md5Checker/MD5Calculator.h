#pragma once

class CMD5Calculator
{
public:
    void Init();
    void Update(const LPBYTE input, UINT inputLen);
    LPBYTE Finalize(BYTE digest[16]);

private:
    void Transform(const BYTE[64]);

private:
    ULONG m_state[4];   // state (ABCD)
    ULONG m_count[2];   // number of bits, modulo 2^64 (lsb first)
    BYTE m_buffer[64];  // input buffer
};
