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

    // For the last ball path: at a node with k balls (initially closed),
    // last direction is left if k is odd, right if k is even.
    // We'll track the path index within the level arrays (path_idx) and
    // the heap index of the node reached (leaf_index).
    int path_idx = 1;          // index within current level counts (1-based)
    int leaf_switch_index = 1; // heap index of leaf switch (level D)

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

        // Determine last ball direction at current node along path
        long long k_path = cur_counts[path_idx];
        int dir_bit = (int)((k_path & 1LL) ? 0 : 1); // odd -> left(0), even -> right(1)
        if (level < (int)D) {
            // Update heap index of the leaf switch reached by decisions up to level D-1
            leaf_switch_index = leaf_switch_index * 2 + dir_bit; // left: *2, right: *2+1
            // Update path_idx for next level in the level-arranged arrays
            path_idx = (dir_bit == 0) ? (path_idx * 2 - 1) : (path_idx * 2);
        }

        // Advance to next level counts
        cur_counts.swap(next_counts);
    }

    // Output
    cout << leaf_switch_index << '\n';
    int leaf_start = 1 << (D - 1);
    int leaf_end = (1 << D) - 1;
    for (int i = leaf_start; i <= leaf_end; ++i) {
        if (i > leaf_start) cout << ' ';
        cout << state[i];
    }
    cout << '\n';

    return 0;
}
