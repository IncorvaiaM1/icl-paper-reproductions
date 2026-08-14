
#include "des.h"

// Initial Permutation table
const unsigned char IP_TABLE[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9,  1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

// Final Permutation (Inverse IP) table
const unsigned char FP_TABLE[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9,  49, 17, 57, 25
};

// Expansion Permutation table
const unsigned char E_TABLE[48] = {
    32, 1,  2,  3,  4,  5,
    4,  5,  6,  7,  8,  9,
    8,  9,  10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

// P-Box Permutation table
const unsigned char P_TABLE[32] = {
    16, 7, 20, 21, 29, 12, 28, 17,
    1,  15, 23, 26, 5,  18, 31, 10,
    2,  8,  24, 14, 32, 27, 3,  9,
    19, 13, 30, 6,  22, 11, 4,  25
};

// Permuted Choice 1 table
const unsigned char PC1_TABLE[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1,  58, 50, 42, 34, 26, 18,
    10, 2,  59, 51, 43, 35, 27,
    19, 11, 3,  60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7,  62, 54, 46, 38, 30, 22,
    14, 6,  61, 53, 45, 37, 29,
    21, 13, 5,  28, 20, 12, 4
};

// Permuted Choice 2 table
const unsigned char PC2_TABLE[48] = {
    14, 17, 11, 24, 1,  5,
    3,  28, 15, 6,  21, 10,
    23, 19, 12, 4,  26, 8,
    16, 7,  27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

// Number of left rotations for each round
const unsigned char LEFT_ROTATIONS[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1
};

void IP(des_state_t *state, des_block_t *in) {
    unsigned int l = 0, r = 0;
    
    // Apply Initial Permutation
    for (int i = 0; i < 32; i++) {
        int bit_pos = IP_TABLE[i] - 1;
        int byte_pos = bit_pos / 8;
        int bit_offset = 7 - (bit_pos % 8);
        l |= (((in[byte_pos] >> bit_offset) & 1) << (31 - i));
    }
    
    for (int i = 0; i < 32; i++) {
        int bit_pos = IP_TABLE[i + 32] - 1;
        int byte_pos = bit_pos / 8;
        int bit_offset = 7 - (bit_pos % 8);
        r |= (((in[byte_pos] >> bit_offset) & 1) << (31 - i));
    }
    
    state[0] = l;
    state[1] = r;
}

void InvIP(des_state_t *state, des_block_t *out) {
    unsigned char temp[8] = {0};
    
    // Apply Final Permutation (Inverse IP)
    for (int i = 0; i < 64; i++) {
        int bit_pos = FP_TABLE[i] - 1;
        int src_byte, src_bit;
        unsigned int src_val;
        
        if (bit_pos < 32) {
            src_byte = bit_pos / 8;
            src_bit = 7 - (bit_pos % 8);
            src_val = (state[0] >> (31 - bit_pos)) & 1;
        } else {
            bit_pos -= 32;
            src_byte = bit_pos / 8;
            src_bit = 7 - (bit_pos % 8);
            src_val = (state[1] >> (31 - bit_pos)) & 1;
        }
        
        int dest_byte = i / 8;
        int dest_bit = 7 - (i % 8);
        temp[dest_byte] |= (src_val << dest_bit);
    }
    
    for (int i = 0; i < 8; i++) {
        out[i] = temp[i];
    }
}

unsigned int f(unsigned int right, des_subkey_t *subkey) {
    unsigned int expanded = 0;
    unsigned int result = 0;
    
    // Expansion Permutation
    for (int i = 0; i < 48; i++) {
        int bit_pos = E_TABLE[i] - 1;
        expanded |= BITNUMINTR(right, bit_pos, 47 - i);
    }
    
    // XOR with subkey
    unsigned int s_input[8];
    unsigned int xor_result = 0;
    for (int i = 0; i < 6; i++) {
        xor_result |= ((expanded >> (42 - 8*i)) & 0xFF) << (40 - 8*i);
    }
    xor_result >>= 16;
    
    for (int i = 0; i < 8; i++) {
        unsigned int byte_val = (xor_result >> (24 - 3*i)) & 0x7F;
        if (i < 6) {
            s_input[i] = ((subkey[i*6/8] << 8) | subkey[i*6/8 + 1]) >> (4 - (i*6 % 8));
            s_input[i] &= 0x3F;
        } else {
            s_input[i] = ((subkey[5] << 8) | subkey[6]) >> (4 - (i*6 % 8));
            s_input[i] &= 0x3F;
        }
    }

    // Manual S-box implementation
    unsigned int sbox_outputs = 0;
    for (int i = 0; i < 8; i++) {
        unsigned int input_val = 0;
        int shift = 42 - i*6;
        if (shift >= 0) {
            input_val = (xor_result >> shift) & 0x3F;
        } else {
            input_val = (xor_result << (-shift)) & 0x3F;
        }

        unsigned int row = ((input_val & 0x20) >> 4) | (input_val & 0x01);
        unsigned int col = (input_val & 0x1E) >> 1;
        unsigned int sbox_val;
        
        switch(i) {
            case 0: sbox_val = sbox1[row * 16 + col]; break;
            case 1: sbox_val = sbox2[row * 16 + col]; break;
            case 2: sbox_val = sbox3[row * 16 + col]; break;
            case 3: sbox_val = sbox4[row * 16 + col]; break;
            case 4: sbox_val = sbox5[row * 16 + col]; break;
            case 5: sbox_val = sbox6[row * 16 + col]; break;
            case 6: sbox_val = sbox7[row * 16 + col]; break;
            case 7: sbox_val = sbox8[row * 16 + col]; break;
            default: sbox_val = 0; break;
        }
        
        sbox_outputs |= (sbox_val << (28 - 4*i));
    }
    
    // P-Box Permutation
    for (int i = 0; i < 32; i++) {
        int bit_pos = P_TABLE[i] - 1;
        result |= BITNUMINTR(sbox_outputs, bit_pos, 31 - i);
    }
    
    return result;
}

void des_key_setup(const unsigned char key[], des_key_t schedule, DES_MODE mode) {
    unsigned int C = 0, D = 0;
    unsigned char key_bits[56];
    
    // PC-1 Permutation
    for (int i = 0; i < 56; i++) {
        int bit_pos = PC1_TABLE[i] - 1;
        int byte_pos = bit_pos / 8;
        int bit_offset = 7 - (bit_pos % 8);
        key_bits[i] = (key[byte_pos] >> bit_offset) & 1;
    }
    
    // Split into C and D (28 bits each)
    for (int i = 0; i < 28; i++) {
        C |= (key_bits[i] << (27 - i));
        D |= (key_bits[i + 28] << (27 - i));
    }
    
    // Generate 16 subkeys
    for (int round = 0; round < 16; round++) {
        // Circular left shift
        C = ((C << LEFT_ROTATIONS[round]) | (C >> (28 - LEFT_ROTATIONS[round]))) & 0x0FFFFFFF;
        D = ((D << LEFT_ROTATIONS[round]) | (D >> (28 - LEFT_ROTATIONS[round]))) & 0x0FFFFFFF;
        
        // PC-2 Permutation to get 48-bit subkey
        unsigned char CD_bits[56];
        for (int i = 0; i < 28; i++) {
            CD_bits[i] = (C >> (27 - i)) & 1;
            CD_bits[i + 28] = (D >> (27 - i)) & 1;
        }
        
        unsigned char subkey_bits[48];
        for (int i = 0; i < 48; i++) {
            subkey_bits[i] = CD_bits[PC2_TABLE[i] - 1];
        }
        
        // Convert bits to bytes for the schedule
        for (int i = 0; i < 6; i++) {
            schedule[round][i] = 0;
            for (int j = 0; j < 8; j++) {
                if (i*8 + j < 48) {
                    schedule[round][i] |= (subkey_bits[i*8 + j] << (7 - j));
                }
            }
        }
    }
    
    // For decryption, reverse the order of subkeys
    if (mode == DES_DECRYPT) {
        des_subkey_t temp;
        for (int i = 0; i < 8; i++) {
            memcpy(temp, schedule[i], 6);
            memcpy(schedule[i], schedule[15 - i], 6);
            memcpy(schedule[15 - i], temp, 6);
        }
    }
}

void des_crypt(des_block_t *in, des_block_t *out, des_key_t *key) {
    des_state_t state;
    
    // Initial Permutation
    IP(state, in);
    
    // 16 Rounds of Feistel Network
    for (int round = 0; round < 16; round++) {
        unsigned int temp = state[0];
        state[0] = state[1];
        state[1] = temp ^ f(temp, (*key)[round]);
    }
    
    // Swap left and right halves for final permutation
    unsigned int temp = state[0];
    state[0] = state[1];
    state[1] = temp;
    
    // Final Permutation
    InvIP(state, out);
}
