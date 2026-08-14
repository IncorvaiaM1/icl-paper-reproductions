
#include "des.h"

// Initial Permutation Table
const unsigned char IP_Table[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// Final Permutation (Inverse of Initial Permutation)
const unsigned char FP_Table[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9,  49, 17, 57, 25
};

// Expansion Permutation Table
const unsigned char E_Table[48] = {
    32, 1,  2,  3,  4,  5,
    4,  5,  6,  7,  8,  9,
    8,  9,  10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

// P-Box Permutation Table
const unsigned char P_Table[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1,  15, 23, 26, 5,  18, 31, 10,
    2,  8,  24, 14, 32, 27, 3,  9,
    19, 13, 30, 6,  22, 11, 4,  25
};

// Permuted Choice 1 Table
const unsigned char PC1_Table[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};

// Permuted Choice 2 Table
const unsigned char PC2_Table[48] = {
    14, 17, 11, 24, 1,  5,
    3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,
    16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

// Number of left shifts for each round
const unsigned char Shift_Table[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

void IP(des_state_t *state, des_block_t *in) {
    unsigned char temp[8];
    memcpy(temp, in, 8);

    unsigned int L = 0, R = 0;

    for (int i = 0; i < 32; i++) {
        int bit_pos = IP_Table[i] - 1;
        int byte_idx = bit_pos / 8;
        int bit_idx = 7 - (bit_pos % 8);
        L |= ((temp[byte_idx] >> bit_idx) & 1) << (31 - i);
    }

    for (int i = 0; i < 32; i++) {
        int bit_pos = IP_Table[i + 32] - 1;
        int byte_idx = bit_pos / 8;
        int bit_idx = 7 - (bit_pos % 8);
        R |= ((temp[byte_idx] >> bit_idx) & 1) << (31 - i);
    }

    (*state)[0] = L;
    (*state)[1] = R;
}

void InvIP(des_state_t *state, des_block_t *out) {
    unsigned int L = (*state)[0];
    unsigned int R = (*state)[1];
    unsigned char temp[8] = {0};

    for (int i = 0; i < 64; i++) {
        int bit_pos = FP_Table[i] - 1;
        int dest_byte = i / 8;
        int dest_bit = 7 - (i % 8);

        unsigned int src_val;
        if (bit_pos < 32) {
            src_val = L;
        } else {
            src_val = R;
            bit_pos -= 32;
        }

        int src_bit = 31 - bit_pos;
        if ((src_val >> src_bit) & 1) {
            temp[dest_byte] |= (1 << dest_bit);
        }
    }

    memcpy(out, temp, 8);
}

unsigned int f(unsigned int state, des_subkey_t *key) {
    unsigned char expanded[6] = {0};
    unsigned char sbox_output[4] = {0};

    // Expansion permutation
    for (int i = 0; i < 48; i++) {
        int bit_pos = E_Table[i] - 1;
        int src_bit = 31 - bit_pos;
        if ((state >> src_bit) & 1) {
            int byte_idx = i / 8;
            int bit_idx = 7 - (i % 8);
            expanded[byte_idx] |= (1 << bit_idx);
        }
    }

    // XOR with key
    for (int i = 0; i < 6; i++) {
        expanded[i] ^= (*key)[i];
    }

    // S-Box substitutions
    for (int s = 0; s < 8; s++) {
        unsigned char src = 0;
        for (int i = 0; i < 6; i++) {
            int bit_pos = s * 6 + i;
            int byte_idx = bit_pos / 8;
            int bit_idx = 7 - (bit_pos % 8);
            if ((expanded[byte_idx] >> bit_idx) & 1) {
                src |= (1 << (5 - i));
            }
        }

        unsigned char row = ((src & 0x20) >> 4) | (src & 0x01);
        unsigned char col = (src & 0x1E) >> 1;
        unsigned char index = (row << 4) | col;

        unsigned char val;
        switch (s) {
            case 0: val = sbox1[index]; break;
            case 1: val = sbox2[index]; break;
            case 2: val = sbox3[index]; break;
            case 3: val = sbox4[index]; break;
            case 4: val = sbox5[index]; break;
            case 5: val = sbox6[index]; break;
            case 6: val = sbox7[index]; break;
            case 7: val = sbox8[index]; break;
            default: val = 0;
        }

        for (int i = 0; i < 4; i++) {
            if ((val >> (3 - i)) & 1) {
                int bit_pos = s * 4 + i;
                int byte_idx = bit_pos / 8;
                int bit_idx = 7 - (bit_pos % 8);
                sbox_output[byte_idx] |= (1 << bit_idx);
            }
        }
    }

    // P-Box permutation
    unsigned int result = 0;
    for (int i = 0; i < 32; i++) {
        int bit_pos = P_Table[i] - 1;
        int byte_idx = bit_pos / 8;
        int bit_idx = 7 - (bit_pos % 8);
        if ((sbox_output[byte_idx] >> bit_idx) & 1) {
            result |= (1 << (31 - i));
        }
    }

    return result;
}

void des_key_setup(const unsigned char key[], des_key_t schedule, DES_MODE mode) {
    unsigned char pc1_key[7] = {0};
    unsigned char c[17][4] = {0};
    unsigned char d[17][4] = {0};

    // PC-1 permutation
    for (int i = 0; i < 56; i++) {
        int bit_pos = PC1_Table[i] - 1;
        int src_byte = bit_pos / 8;
        int src_bit = 7 - (bit_pos % 8);
        if ((key[src_byte] >> src_bit) & 1) {
            int dest_byte = i / 8;
            int dest_bit = 7 - (i % 8);
            pc1_key[dest_byte] |= (1 << dest_bit);
        }
    }

    // Split into C and D
    for (int i = 0; i < 28; i++) {
        int bit_pos = i;
        int src_byte = bit_pos / 8;
        int src_bit = 7 - (bit_pos % 8);
        if ((pc1_key[src_byte] >> src_bit) & 1) {
            int dest_byte = i / 8;
            int dest_bit = 7 - (i % 8);
            c[0][dest_byte] |= (1 << dest_bit);
        }
    }

    for (int i = 0; i < 28; i++) {
        int bit_pos = i + 28;
        int src_byte = bit_pos / 8;
        int src_bit = 7 - (bit_pos % 8);
        if ((pc1_key[src_byte] >> src_bit) & 1) {
            int dest_byte = i / 8;
            int dest_bit = 7 - (i % 8);
            d[0][dest_byte] |= (1 << dest_bit);
        }
    }

    // Key schedule generation
    for (int round = 1; round <= 16; round++) {
        int shift = Shift_Table[round - 1];

        // Left shift C
        unsigned int c_temp = (c[round-1][0] << 24) | (c[round-1][1] << 16) | (c[round-1][2] << 8) | c[round-1][3];
        c_temp <<= shift;
        if (shift == 2) {
            if (c[round-1][3] & 0x80) c_temp |= 0x01000000;
            if (c[round-1][3] & 0x40) c_temp |= 0x02000000;
        } else {
            if (c[round-1][3] & 0x80) c_temp |= 0x01000000;
        }
        c_temp &= 0x0FFFFFFF;
        c[round][0] = (c_temp >> 24) & 0xFF;
        c[round][1] = (c_temp >> 16) & 0xFF;
        c[round][2] = (c_temp >> 8) & 0xFF;
        c[round][3] = c_temp & 0xFF;

        // Left shift D
        unsigned int d_temp = (d[round-1][0] << 24) | (d[round-1][1] << 16) | (d[round-1][2] << 8) | d[round-1][3];
        d_temp <<= shift;
        if (shift == 2) {
            if (d[round-1][3] & 0x80) d_temp |= 0x01000000;
            if (d[round-1][3] & 0x40) d_temp |= 0x02000000;
        } else {
            if (d[round-1][3] & 0x80) d_temp |= 0x01000000;
        }
        d_temp &= 0x0FFFFFFF;
        d[round][0] = (d_temp >> 24) & 0xFF;
        d[round][1] = (d_temp >> 16) & 0xFF;
        d[round][2] = (d_temp >> 8) & 0xFF;
        d[round][3] = d_temp & 0xFF;

        // Combine C and D and apply PC-2
        unsigned char cd[7];
        cd[0] = c[round][0];
        cd[1] = c[round][1];
        cd[2] = c[round][2];
        cd[3] = c[round][3];
        cd[4] = d[round][0];
        cd[5] = d[round][1];
        cd[6] = d[round][2];

        for (int i = 0; i < 48; i++) {
            int bit_pos = PC2_Table[i] - 1;
            int src_byte = bit_pos / 8;
            int src_bit = 7 - (bit_pos % 8);
            if ((cd[src_byte] >> src_bit) & 1) {
                int dest_byte = i / 8;
                int dest_bit = 7 - (i % 8);
                schedule[round-1][dest_byte] |= (1 << dest_bit);
            }
        }
    }

    // For decryption, reverse the key schedule
    if (mode == DES_DECRYPT) {
        des_key_t temp_schedule;
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 6; j++) {
                temp_schedule[i][j] = schedule[15 - i][j];
            }
        }
        memcpy(schedule, temp_schedule, sizeof(des_key_t));
    }
}

void des_crypt(des_block_t *in, des_block_t *out, des_key_t *key) {
    des_state_t state;
    IP(&state, in);

    unsigned int L, R, temp;
    L = state[0];
    R = state[1];

    for (int round = 0; round < 16; round++) {
        temp = R;
        R = L ^ f(R, &(*key)[round]);
        L = temp;
    }

    state[0] = R;
    state[1] = L;

    InvIP(&state, out);
}
