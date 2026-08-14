
#include "des.h"

// Initial Permutation (IP) table (0-indexed)
static const int IP_TABLE[64] = {
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7,
    56, 48, 40, 32, 24, 16, 8, 0,
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6
};

// Inverse Initial Permutation (InvIP) table (0-indexed)
static const int INV_IP_TABLE[64] = {
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25,
    32, 0, 40, 8, 48, 16, 56, 24
};

// Expansion Permutation (E) table (0-indexed)
static const int E_TABLE[48] = {
    31, 0, 1, 2, 3, 4,
    3, 4, 5, 6, 7, 8,
    7, 8, 9, 10, 11, 12,
    11, 12, 13, 14, 15, 16,
    15, 16, 17, 18, 19, 20,
    19, 20, 21, 22, 23, 24,
    23, 24, 25, 26, 27, 28,
    27, 28, 29, 30, 31, 0
};

// P-Box Permutation table (0-indexed)
static const int P_TABLE[32] = {
    15, 6, 19, 20,
    28, 11, 27, 16,
    0, 14, 22, 25,
    4, 17, 30, 9,
    1, 7, 23, 13,
    31, 26, 2, 8,
    18, 12, 29, 5,
    21, 10, 3, 24
};

// Perform initial permutation on input block to generate state
void IP(des_block_t *in, des_state_t state) {
    unsigned int left = 0;
    unsigned int right = 0;
    
    for (int i = 0; i < 64; i++) {
        int pos = IP_TABLE[i];
        int byte_index = pos / 8;
        int bit_index = 7 - (pos % 8);
        unsigned int bit = (in[byte_index] >> bit_index) & 0x01;
        
        if (i < 32) {
            left |= (bit << (31 - i));
        } else {
            right |= (bit << (63 - i));
        }
    }
    
    state[0] = left;
    state[1] = right;
}

// Perform inverse initial permutation on state to generate output block
void InvIP(des_state_t state, des_block_t *out) {
    memset(out, 0, DES_BLOCK_SIZE);
    
    for (int i = 0; i < 64; i++) {
        int pos = INV_IP_TABLE[i];
        unsigned int bit;
        
        if (pos < 32) {
            bit = (state[0] >> (31 - pos)) & 0x01;
        } else {
            bit = (state[1] >> (63 - pos)) & 0x01;
        }
        
        int byte_index = i / 8;
        int bit_index = 7 - (i % 8);
        out[byte_index] |= (bit << bit_index);
    }
}

// Feistel function (f) for DES
unsigned int f(unsigned int R, unsigned char subkey[6]) {
    // Expansion permutation: 32-bit to 48-bit
    unsigned char expanded[6] = {0};
    for (int i = 0; i < 48; i++) {
        int pos = E_TABLE[i];
        unsigned int bit = (R >> (31 - pos)) & 0x01;
        int byte_index = i / 8;
        int bit_index = 7 - (i % 8);
        expanded[byte_index] |= (bit << bit_index);
    }
    
    // XOR with subkey
    for (int i = 0; i < 6; i++) {
        expanded[i] ^= subkey[i];
    }
    
    // S-Box substitution
    unsigned int sbox_output = 0;
    const unsigned char* sboxes[8] = {sbox1, sbox2, sbox3, sbox4, sbox5, sbox6, sbox7, sbox8};
    
    for (int j = 0; j < 8; j++) {
        int start_bit = j * 6;
        unsigned char group = 0;
        
        for (int k = 0; k < 6; k++) {
            int bit_pos = start_bit + k;
            int byte_index = bit_pos / 8;
            int bit_index = 7 - (bit_pos % 8);
            unsigned char bit = (expanded[byte_index] >> bit_index) & 0x01;
            group = (group << 1) | bit;
        }
        
        unsigned char index = SBOXBIT(group);
        unsigned char s_val = sboxes[j][index];
        sbox_output = (sbox_output << 4) | s_val;
    }
    
    // P-Box permutation
    unsigned int pbox_output = 0;
    for (int i = 0; i < 32; i++) {
        int pos = P_TABLE[i];
        unsigned int bit = (sbox_output >> (31 - pos)) & 0x01;
        pbox_output |= (bit << (31 - i));
    }
    
    return pbox_output;
}

// Main DES encryption function
void des_crypt(des_block_t *in, des_block_t *out, des_key_t *key) {
    des_state_t state;
    
    // Initial permutation
    IP(in, state);
    
    unsigned int L = state[0];
    unsigned int R = state[1];
    
    // 16 rounds of Feistel network
    for (int i = 0; i < 16; i++) {
        unsigned int nextL = R;
        unsigned int nextR = L ^ f(R, (*key)[i]);
        L = nextL;
        R = nextR;
    }
    
    // Prepare state for inverse permutation (R_16, L_16)
    state[0] = R;
    state[1] = L;
    
    // Inverse initial permutation
    InvIP(state, out);
}
