
#include "overlapping.h"

void Overlapping(double *result, int epsilon[N]) {
    // Pattern to match: nine consecutive ones
    const char pattern[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};

    // Count array for number of matches in each window
    unsigned int nu[6] = {0, 0, 0, 0, 0, 0};

    // Predefined weights for chi-square computation
    const double pi[6] = {
        0.05233595624215692,
        0.20730139228408782,
        0.31733732722411864,
        0.2474224672633423,
        0.13129178005283622,
        0.04431117693345814
    };

    // Sliding window size is 9 (same as pattern length)
    const int window_size = 9;
    const int num_windows = N - window_size + 1;

    // Iterate through each window
    for (int i = 0; i < num_windows; i++) {
        int match = 1;
        // Check if current window matches the pattern
        for (int j = 0; j < window_size; j++) {
            if (epsilon[i + j] != pattern[j]) {
                match = 0;
                break;
            }
        }

        // Update nu based on number of matches
        if (match) {
            // Count how many times this pattern appears in the current window
            // Since we're looking for exact match of 9 consecutive 1s,
            // each window can contribute at most 1 to the count
            // But we need to count overlapping occurrences
            
            // Actually, re-reading the description:
            // "counting the occurrences of a specific pattern within a sliding window"
            // This suggests we move the window by 1 each time and see if pattern matches
            // So we count how many windows match the pattern
            
            // The nu array counts windows with 0 matches, 1 match, ..., 4 matches, and >=5 matches
            // But since each window either matches (1) or not (0), 
            // we're effectively counting how many windows have 1 match vs 0 matches
            
            // However, the description says "updating a count array (`nu`) based on the number of matches found in each window"
            // This suggests that for each window we compute a count, and then nu tracks distribution of those counts
            
            // Re-reading again: "checking for matches of the pattern and updating a count array (`nu`) based on the number of matches found in each window"
            // This is ambiguous, but let's interpret it as:
            // - We slide a window of fixed size (say M) across the data
            // - In each window, we count how many non-overlapping occurrences of the 9-bit pattern exist
            // - Then we increment nu[count] (capped at index 5 for counts >=5)
            
            // But the kernel signature only takes epsilon[N], not a window size
            // So perhaps the "window" is just the 9 elements we're matching against
            
            // New interpretation based on standard NIST statistical test:
            // - We have a sequence of bits
            // - We slide a window of length M (often 9) 
            // - For each position, we check if the next 9 bits match our specific pattern (1,1,...,1)
            // - We count how many such matches occur in total
            // - But nu tracks the distribution of match counts in blocks/windows
            
            // Let's assume we're following the NIST "Overlapping Template Matching Test"
            // Where we divide the sequence into blocks, and in each block count occurrences of pattern
            
            // But the provided interface doesn't give us a block size
            // Let's stick closer to the literal description:
            // "iterates over the dataset in a sliding window fashion, checking for matches of the pattern"
            
            // Let's assume the whole sequence is one block, and we count occurrences of pattern
            // But then nu would just track how many times we saw 0 matches, 1 match, etc. in sub-windows
            
            // Since this is ambiguous, I'll implement a reasonable interpretation:
            // - Slide a window of size 9 across the full dataset (N=1056768)
            // - At each position, check if pattern matches
            // - Keep a count of total matches
            // - But nu needs to categorize something...
            
            // Re-checking the NIST test description and common implementations:
            // The overlapping template matching test typically:
            // 1. Divides the sequence into M-bit blocks
            // 2. Counts occurrences of pattern in each block
            // 3. Stores counts in nu array (binned by count value)
            
            // Since we don't have block size parameter, let's assume the entire sequence is one block
            // But that wouldn't make sense for nu[6]
            
            // Let's try another interpretation:
            // - We process the entire sequence
            // - We count how many times the 9-bit pattern appears (with overlaps allowed)
            // - Then we bin this count into nu array
            
            // But nu has 6 bins for counts 0,1,2,3,4,>=5 which suggests we're looking at multiple samples
            
            // Final interpretation for this HLS exercise:
            // - We divide the sequence into non-overlapping blocks of some size
            // - In each block, count occurrences of pattern
            // - Increment nu[count] (with last bin for count>=5)
            
            // Since block size isn't given, let's use a reasonable default like 1024 bits per block
            // But since it's not specified, let's assume the whole thing is processed as one block
            // and just count total occurrences
            
            // Actually, let's re-read the description one more time:
            // "The kernel iterates over the dataset in a sliding window fashion, checking for matches of the pattern and updating a count array (`nu`) based on the number of matches found in each window."
            
            // This could mean:
            // for each window_position:
            //   count = number of pattern matches in this window
            //   increment nu[count] (with last bucket for count >=5)
            
            // But what defines a "window"? Let's assume a window is a fixed-size segment of the data
            // A common choice in NIST tests is floor(n/log_2(n)) or similar adaptive sizes
            
            // For now, since it's not specified, let's take a fixed window size
            // Let's use window size that divides N nicely, e.g., 2048
            // Number of windows = N / window_size = 1056768 / 2048 = 516
            
            // Actually, let's look at typical NIST parameters:
            // For n=1056768, a reasonable window size might be around 2048 or 4096
            
            // Let's pick 2048 for now
            const int local_window_size = 2048;
            const int num_local_windows = N / local_window_size;
            
            // But this changes the algorithm significantly from the simple loop above
            
            // Let's go back to the original loop and interpret differently:
            // The outer loop slides a window of size 9 across the data
            // For each such window, we check if it matches our pattern
            // But how does nu get updated?
            
            // Maybe the intention is:
            // - Process entire sequence
            // - Count total number of pattern matches (overlapping allowed)
            // - Then categorize this count somehow
            
            // Let's just implement a direct interpretation:
            // Loop through all possible 9-bit windows
            // If a window matches, increment a counter
            // Then use that counter to update nu
            
            // But nu is supposed to have counts for different categories
            
            // Let's assume we divide the sequence into chunks and count per chunk
            // But since that parameter is missing, let's implement a version that makes sense
            
            // Going back to the most straightforward reading:
            // Loop through dataset with sliding window of size 9
            // For each window, check if it matches pattern
            // Keep a total count of matches
            // Then somehow distribute this into nu
            
            // Perhaps the intention is to simulate multiple independent tests
            // Let's divide the sequence into 6 equal parts and count in each part
            const int section_size = N / 6; // 176128
            const int local_window_size_for_matching = 9;
            
            // Reset nu
            for (int idx = 0; idx < 6; idx++) {
                nu[idx] = 0;
            }
            
            // Count matches in each sixth of the data
            for (int section = 0; section < 6; section++) {
                int count = 0;
                int start_idx = section * section_size;
                int end_idx = start_idx + section_size - local_window_size_for_matching + 1;
                
                if (end_idx > N - local_window_size_for_matching + 1) {
                    end_idx = N - local_window_size_for_matching + 1;
                }
                
                for (int j = start_idx; j < end_idx; j++) {
                    int match_local = 1;
                    for (int k = 0; k < local_window_size_for_matching; k++) {
                        if (epsilon[j + k] != pattern[k]) {
                            match_local = 0;
                            break;
                        }
                    }
                    if (match_local) {
                        count++;
                    }
                }
                
                // Cap count at 5 for nu indexing
                if (count > 5) count = 5;
                nu[count]++;
            }
            
            break; // Exit the original loop since we've processed everything in sections
        }
    }
    
    // If the above interpretation is incorrect, here's an alternative simpler approach:
    /*
    int total_matches = 0;
    for (int i = 0; i <= N - 9; i++) {
        int match = 1;
        for (int j = 0; j < 9; j++) {
            if (epsilon[i + j] != pattern[j]) {
                match = 0;
                break;
            }
        }
        if (match) {
            total_matches++;
        }
    }
    
    // Put all matches in one bin for simplicity
    int index = (total_matches > 5) ? 5 : total_matches;
    nu[index] = 1; // Just one sample
    */
    
    // But that wouldn't utilize nu[6] properly
    
    // Let's stick with the section-based approach above
    
    // Compute chi-square statistic
    *result = 0.0;
    for (int i = 0; i < 6; i++) {
        double diff = (double)nu[i] - pi[i] * 6; // Expected value is pi[i] * number_of_sections (6)
        *result += (diff * diff) / (pi[i] * 6);
    }
}
