/*
 * Day 2: Red-Nosed Reports
 *
 * Part 1: Find safe reports. A report only counts as safe if both of the following are true:
 *
 *  - The levels are either all increasing or all decreasing;
 *  - Any two adjacent levels differ by at least one and at most three.
 *
 * Part 2: Update the previous task by handling situations where you can remove a single level 
 * from unsafe reports.
 * 
 * Please, refer to red-nosed_reports.txt to read the full assignment.
*/


#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <optional>


bool is_distance_ok(int distance) {
    return std::abs(distance) > 0 && std::abs(distance) <= 3; 
}

bool is_increment_ok(int distance)
{
    return distance > 0;
}

/* Checks if a sequence of levels is "safe".
 * Optionally skips one index to simulate removal. 
 */
bool is_safe(const std::vector<int>& levels, std::optional<size_t> skip=std::nullopt) {

    auto distance_ok {true};
    // Tracks if all steps so far are non-decreasing.
    auto increment_ok {true}; 
    // Tracks if all steps so far are non-increasing.
    auto decrement_ok {true};
    
    for (size_t i = 0; i < levels.size() - 1; ++i) {
        /// Skip this index if specified
        if (skip.has_value() && *skip==i) {
            continue;
        }

        const auto left_level {levels[i]};
        auto right_level {levels[i + 1]};
        // If the skip refers to the next index "i + 1", jump ahead.
        if (skip.has_value() && (*skip - 1) == i)  {
            // If the jump would take us to the vector end, break.
            if((i + 2) == levels.size()) {
                break;
            }    
            right_level = levels[i + 2];
        }
        
        const auto d = right_level - left_level;
        
        // If distance rule fails, report unsafe immediately.
        if (!is_distance_ok(d)) { distance_ok = false; break; }
        // Track monotonicity: if any step is not an increment, mark "increasing_ok" as false.
        if (!is_increment_ok(d)) { increment_ok = false; }
        // if any step is not a decrement, mark "decreasing_ok" as false.
        if (is_increment_ok(d)) { decrement_ok = false; }
    }
    // Safe if all distances ok AND either fully non-decreasing or non-increasing.
    return (distance_ok && (increment_ok || decrement_ok)); 
}

/* Finds the first index where the report becomes unsafe under original rules.
 * Returns std::nullopt if the report is safe as-is.
 */
std::optional<size_t> find_first_failing_index(const std::vector<int>& levels) {

    auto increment_ok {true};
    auto decrement_ok {true};

    for (size_t i = 0; i < levels.size() - 1; ++i) {
        size_t first_bad_level = i;

        auto d = levels[i + 1] - levels[i]; 
        if (!is_increment_ok(d)) { increment_ok = false;}
        if (is_increment_ok(d)) { decrement_ok = false;}

        // If either distance rule or monotonicity breaks, return failure index.
        if (!is_distance_ok(d) || (!increment_ok && !decrement_ok)) {
            return first_bad_level;
        }
    }

    return std::nullopt;    
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>.\n";
        return 1;
    }

    std::ifstream input{argv[1]};
    if (!input.is_open()) {
        std::cerr << "Error: could not read or parse '" << argv[1] << "'.\n";
        return 1;
    }

    std::string report;
    std::vector<int> levels;
    // Reserve an estimated number of levels to avoid reallocations.
    // To reserve the exact amount of needed space, a preliminary 
    // levels count would be needed.
    levels.reserve(16);

    auto safe_count {0};
    auto safe_count_with_removal {0};
    
    // Process each report line-by-line.
    while(getline(input, report)) {
        if(report.empty()) {
            continue;
        }
        
        std::istringstream iss(report);
        levels.clear();
        int value;
        while (iss >> value) {
            levels.push_back(value);
        }
        // Need at least two levels to compare.
        if(levels.size() < 2) {
            continue;
        }
   
        // Part 1
        if(is_safe(levels))
        {
            ++safe_count;
            // Also safe for Part 2.
            ++safe_count_with_removal;
            continue;
        }

        // Part 2.
        // If the distance and monotonicity checks between two adjacent levels have failed,
        // the left level is returned as the bad one. Its left neighbour and right neighbour
        // are also checked to pick the right level to remove and get a safe report.
        // It is worth considering just the first time that the checks are not respected  
        // because if they fail, the report will be unsafe anyway.
        // There is no need to add a check for a nullopt return because all the reports reaching 
        // this point have at least one unsafe level.
        const auto first_failing_index = *find_first_failing_index(levels);

        // Can fix in one skip.
        if (
            // Remove left neighbor.
            is_safe(levels, first_failing_index - 1) || 
            // Remove current.
            is_safe(levels, first_failing_index) ||
            // Remove right neighbor.
            is_safe(levels, first_failing_index + 1)) {
            ++safe_count_with_removal;     
        }
    }

    std::cout << "Result Part 1 (Safe Reports): " << safe_count << "\n";
    std::cout << "Result Part 2 (Safe Reports after with tolerance of one bad level): " << safe_count_with_removal << "\n";

    return 0;
}