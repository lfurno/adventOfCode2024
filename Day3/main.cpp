/*
 * Day 3: Mull It Over
 * 
 * Part 1: Scan the corrupted memory for uncorrupted mul instructions.
 * An uncorrupted mul instruction is of the type mul(X, Y) where
 * X and Y are each 1-3 digits numbers. Then, add up the result of the
 * multiplication X * Y.
 *  
 * Part 2: Scan the corrupted memory for uncorrupted mul instructions, 
 * taking into account
 * - The do() instruction that enables future mul instructions.
 * - The don't() instruction that disables future mul instructions.
 * Then, add up the result of the multiplication X * Y, if mul is 
 * enabled.
 * 
 * Please, refer to mull_it_over.txt to read the full assignment.
*/



#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string_view>
#include <charconv>


/*
 * Reads the entire file "filename" into "instructions". 
 * Returns true on success.
 */
bool parse_input(const std::string& filename,
                std::string& instructions) {
    std::ifstream input {filename};
    if(!input) {
        std::cerr << "Error: cannot open " << filename << "\n";
        return false;
    }
    
    // Reserve space for performance based on file size.
    const auto file_size {std::filesystem::file_size(filename)}; 
    instructions.clear();
    instructions.reserve(file_size);

    instructions.assign(std::istreambuf_iterator<char>(input),
                          std::istreambuf_iterator<char>());

    if (input.bad()) {
        return false;
    }

    return true;
}

// Parses an integer from a string_view, throws on invalid number.
int64_t parse_num(std::string_view sv) {
    int64_t x {};
    auto [_, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), x);
    if (ec != std::errc()) {
        throw std::runtime_error("Invalid number: \"" + std::string(sv) + "\"");
    }
    return x;
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>.\n";
        return 1;
    }

    std::string instructions; 
    if (!parse_input(argv[1], instructions)) {
        std::cerr << "Error: could not read or parse '" << argv[1] << "'.\n";
        return 1;
    }
    if (instructions.empty()) {
      std::cerr << "Instructions is empty.\n";
      return 1;  
    }
   
    int64_t sum_mul {0};
    int64_t sum_mul_if_enabled {0};

    constexpr std::string_view key_mul {"mul("};
    constexpr std::string_view key_sep {","};
    constexpr std::string_view key_close {")"};
    constexpr std::string_view key_enable {"do("};
    constexpr std::string_view key_disable {"don't("};
    
    // mul instructions start enabled.
    bool enabled {true};

    size_t pos {0};
    const std::string_view instructions_sv {instructions};
    // Part 1 - 2
    while (pos<instructions_sv.size()) {
        // Toggle enabled/disabled state.
        if(instructions_sv.compare(pos, key_enable.size(), key_enable) == 0) {
            enabled = true;
        }
        if(instructions_sv.compare(pos, key_disable.size(), key_disable) == 0) {
            enabled = false;
        }

        // Found a 'mul(' sequence: attempt to parse.
        if(instructions_sv.compare(pos, key_mul.size(), key_mul) == 0) {
            const auto start {pos + key_mul.size()};

            // Find the comma between the two numbers.
            const auto separator {instructions_sv.find(key_sep, start)};
            if(separator == std::string_view::npos) {
                ++pos;
                continue;
            }

            // Limit multiplicand to 3 digits.
            const auto len1 {separator - start};
            if(len1 == 0 || len1 > 3) {
                pos = start;
                continue;
            }
            const auto x {parse_num(instructions_sv.substr(start, len1))}; 

            // Find closing parenthesis.
            const auto close {instructions_sv.find(key_close, separator + 1)};
            if(close == std::string_view::npos) {
                ++pos;
                continue;
            }

            // Limit multiplier to 3 digits.
            const auto len2 {close - (separator + 1)};
            if(len2 == 0 || len2 > 3) {
                pos = separator + 1;
                continue;
            }
            const auto y {parse_num(instructions_sv.substr(separator + 1, len2))};
          
            sum_mul += x * y;
            
            if (enabled) {
                sum_mul_if_enabled += x * y;
            }
           
            // Advance past the ')'
            pos = close + 1; 
        }
        else {
            ++pos;
        }
    }

    std::cout << "Result Part 1 (Uncorrupted muls summation): " << sum_mul << "\n";
    std::cout << "Result Part 2 (Uncorrupted and enabled muls summation): " << 
        sum_mul_if_enabled << "\n";

    return 0;
}