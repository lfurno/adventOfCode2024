/*
 * Day 1: Historian Hysteria
 *
 * Part 1: Compute the total distance between a given left list and a given right 
 * list, add up the distances between all of the pairs you found. 
 * 
 * Part 2: Calculate a total similarity score by adding up each number in the left 
 * list after multiplying it by the number of times that number appears in the right list.
 * 
 * Please, refer to historian_hysteria.txt to read the full assignment.
*/


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>


/*
 * Reads pairs of ints from "filename" into "left" and "right".
 * Returns true on success.
 */
bool parse_input(const std::string& filename,
                std::vector<int>& left,
                std::vector<int>& right)
{
    std::ifstream input{filename};
    if(!input) {
        return false;
    }
    
    // To reserve the exact amount of needed space, a
    // preliminary line-count pass would be needed.
    left.clear();
    left.reserve(100000); 
    right.clear();
    right.reserve(100000);

    int ll, rl;
    while (input >> ll >> rl) {
      left.push_back(ll);
      right.push_back(rl);
    }

    if (input.bad()) {
        return false;
    }

    return true;
}

long long compute_total_distance(const std::vector<int>& left,
                                 const std::vector<int>& right) {
    auto init {0ll};

    return std::transform_reduce(
        left.begin(), left.end(), right.begin(),
        init, std::plus<long long>(), 
        [](int l, int r){
            return std::llabs(
                static_cast<long long>(l) - static_cast<long long>(r));
            });
}

long long compute_similarity_score(const std::vector<int>& left,
                                   const std::vector<int>& right) {
    size_t i {0}, j {0};
    auto similarity_score {0ll};
    // Continue until one list is fully traversed.
    while (i < left.size() && j < right.size()) {
        // left[i] can’t match anything in right.
        if (left[i] < right[j]) {
            ++i;
            continue;
        }
        // right[i] can’t match anything in left.
        if (left[i] > right[j]) {
            ++j;
            continue;
        }
        if (left[i] == right[j]) {
            auto freq {0};
            // Count how many times "left[i]" repeats in right[...]
            while((j < right.size()) && (left[i] == right[j])) {
                ++freq;
                ++j;
            }
            similarity_score+=static_cast<long long>(left[i]) * freq;
            ++i;
        }
    }
    
    return  similarity_score;
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>.\n";
        return 1;
    }

    std::vector<int> left, right;
    if (!parse_input(argv[1], left, right)) {
        std::cerr << "Error: could not read or parse '" << argv[1] << "'.\n";
        return 1;
    }

    if (left.empty()) {
      std::cerr << "Left list is empty.\n";
      return 1;  
    }

    if (right.empty()) {
      std::cerr << "Right list is empty.\n";
      return 1;  
    }
      
    if (left.size() != right.size()) {
        std::cerr << "Mismatched number of entries in the input file.\n";
        return 1;
    }

    // Part 1
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());
    const auto total_distance {compute_total_distance(left, right)};   
 
    // Part 2
    const auto similarity_score {compute_similarity_score(left, right)};
    
    std::cout << "Result Part 1 (Total Distance): " << total_distance << "\n";
    std::cout << "Result Part 2 (Similarity Score): " << similarity_score << "\n";

    return 0;
}