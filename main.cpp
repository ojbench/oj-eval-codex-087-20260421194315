#include <bits/stdc++.h>
using namespace std;

// Problem 087 - Falling Balls (ACMOJ 2056)
// Efficient simulation using batch counting per node.
// Outputs:
//  - First line: endpoint index (1..2^D) for the last ball (left-to-right order)
//  - Second line: states of leaf switches (indices 2^{D-1}..2^{D}-1), left to right

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long D, n;
    if (!(cin >> D >> n)) {
        return 0;
    }

    // Total nodes in a full binary tree depth D: 2^D - 1
    int total_nodes = (1 << D) - 1;
    vector<int> state(total_nodes + 1, 0); // 1-indexed, 0=closed,1=open

    // We simulate ball counts per node per level using the known toggling pattern.
    // counts[level][index_at_level], where index_at_level ranges 1..2^{level-1} for levels 1..D
    // But we can keep only current level counts.
    vector<long long> cur_counts(1, 0); // dummy index 0 unused
    cur_counts.resize(2); // level 1 has 1 node at index 1
    cur_counts[1] = n;

    // For endpoint for the last ball, we traverse using parity rule:
    // At a node with k balls arriving initially closed, first ball goes left, toggles open; subsequent alternate.
    // The last ball direction: left if k is odd, right if k is even.
    // We can compute for the last ball by walking levels using these counts.
    int node = 1;
    int endpoint_idx_bits = 0; // left=0, right=1 bits

    for (int level = 1; level <= (int)D; ++level) {
        // Prepare next level counts vector
        vector<long long> next_counts((size_t)(1 << level) + 1, 0); // indices 1..2^{level}

        int level_nodes = 1 << (level - 1);
        for (int idx = 1; idx <= level_nodes; ++idx) {
            long long k = cur_counts[idx];
            // Initial state is closed (0). For k balls:
            // left_count = (k + 1) / 2, right_count = k / 2
            long long left_count = (k + 1) / 2;
            long long right_count = k / 2;

            // Update switch state at this tree node index in full tree numbering
            // Map (level, idx) to full index: start_index = 2^{level-1}, full_index = start_index + (idx - 1)
            int start_index = 1 << (level - 1);
            int full_index = start_index + (idx - 1);
            // After k toggles starting from 0, final state is k % 2
            state[full_index] = (int)(k & 1LL);

            // Distribute counts to children for next level
            int left_child_idx = idx * 2 - 1;
            int right_child_idx = idx * 2;
            next_counts[left_child_idx] += left_count;
            next_counts[right_child_idx] += right_count;
        }

        // Determine last ball direction at the path node for endpoint index bits
        // We need the count k at the specific node along the last ball path.
        // To track this, we also need to know the (level, idx) of the node we are at.
        // We can derive idx along the path using previous decisions. Maintain path_idx_at_level.
        static vector<int> path_idx; // reused across runs; but here only once
        if (level == 1) path_idx.assign((size_t)D + 1, 0);
        if (level == 1) path_idx[level] = 1;
        else {
            // compute idx based on previous bit
            int prev_idx = path_idx[level - 1];
            int bit = (endpoint_idx_bits >> (level - 2)) & 1; // previous decision
            path_idx[level] = prev_idx * 2 - (bit == 0 ? 1 : 0) + (bit == 1 ? 0 : 0);
            // Actually easier: if bit 0 (left), new idx = prev_idx*2-1; if bit 1 (right), new idx = prev_idx*2
            path_idx[level] = (bit == 0) ? (prev_idx * 2 - 1) : (prev_idx * 2);
        }

        long long k_path = cur_counts[path_idx[level]];
        int dir_bit = (int)((k_path & 1LL) ? 0 : 1); // odd -> left(0), even -> right(1)
        endpoint_idx_bits = (endpoint_idx_bits << 1) | dir_bit;

        // Advance to next level counts
        cur_counts.swap(next_counts);
    }

    int endpoint_index = endpoint_idx_bits + 1; // left-to-right numbering

    // Output
    cout << endpoint_index << '\n';
    int leaf_start = 1 << (D - 1);
    int leaf_end = (1 << D) - 1;
    for (int i = leaf_start; i <= leaf_end; ++i) {
        if (i > leaf_start) cout << ' ';
        cout << state[i];
    }
    cout << '\n';

    return 0;
}

