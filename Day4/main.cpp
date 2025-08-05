/*
Day 4: Ceres Search

 * Part 1: Count all occurrences of "XMAS" in any of 8 directions.
 * Part 2: Count all "X-MAS" shapes (two "MAS" diagonals crossing at 'A').
 * 
 * Please, refer to ceres_search.txt to read the full assignment.
*/


#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
#include <string_view>
#include <ranges>


/*
 * Read non-empty lines from "filename" into "grid". 
 * Returns true on success.
 */
bool parse_input(const std::string& filename,
                std::vector<std::string>& grid) {
    std::ifstream input {filename};
    if(!input) {
        return false;
    }
    
    // Reserve space for performance based on file size
    const auto file_size {std::filesystem::file_size(filename)};
    grid.clear(); 
    grid.reserve(file_size);

    std::string line;
    while(getline(input, line)) {
        if(line.empty()) {
            continue;
        }
        grid.push_back(line);
    }

    if (input.bad()) {
        return false;
    }

    return true;
}

/* Match "word" from (r,c) stepping by (r_dir, c_dir).
 * Returns true on full match.
 *
 * Please, use C++20 to make the code work. 
 */
bool match_search(const std::vector<std::string>& grid, 
                  int r, int r_dir, int c, int c_dir, 
                  int row_sz, int cols_sz, 
                  std::string_view word) {

    namespace views = std::views;
    
    auto indices = std::views::iota(0, (int)word.size());
    
    const auto found_word = indices | std::views::transform([&](int i) { 
        const auto rr = r + i * r_dir, cc = c + i * c_dir;
        if(rr < 0 || cc < 0 || rr >= row_sz || cc >= cols_sz) {
           
            return '\0';
        }

        return grid[rr][cc];
    });
    
    return std::equal(word.begin(), word.end(), found_word.begin(), found_word.end());       
}

// Generic word-count in any of 8 dirs.
int count_occurrences(const std::vector<std::string>& grid,
                     std::string_view word,
                     const std::vector<std::pair<int,int>>& dirs) {
    const int rows {int(grid.size())};
    const int cols {int(grid[0].size())};
    
    int count {0};
   
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (grid[r][c] != word[0]) {
                continue;
            }
            // Check all directions
            for (auto [r_dir, c_dir]: dirs) {
                if(match_search(grid, r, r_dir, c, c_dir, rows, cols, word)) {
                    ++count;
                } 
            }
        }
    }
        
    return count;  
}

// Count X-MAS shapes: two "MAS" diagonals crossing at 'A'.
int count_xmas_shapes(const std::vector<std::string>& grid,
                    std::string_view word,
                    const std::vector<std::pair<int,int>>& dirs) {
                        
    const int rows {int(grid.size())};
    const int cols {int(grid[0].size())};
    int count {0};
    for (size_t r = 1; r < rows; ++r) {
        for (size_t c = 1; c < cols; ++c) {
            // Only cells centered in "A" can form a X-MAS shape.
            if (grid[r][c] != 'A') {
                continue;
            }
            
            // We need two "MAS" arms crossing at "A", one along NW or SE, 
            // AND one along NE or SW. For each diagonal, we try both forward 
            // and backward directions.
            // For example, 
            // 
            //  M.S    
            //  .A. 
            //  M.S
            // 
            // is a valid X-MAS shape because a match is found when
            // - moving from the bottom-left position (BL) with respect to "A", 
            //   and following the NE direction;
            // 
            //   ..↗             
            //   .↗.
            //  BL..
            // 
            // - moving from the top-left position (TL) with respect to "A", 
            //   and following the SE direction.                 
            // 
            // TL..            
            // .↘.  
            // ..↘   
                         
            // Check NE-SW diagonal: from bottom-right toward top-left (↖) or vice versa (↘).
            const auto diag1 = (
                // start at BR, go NW.
                match_search(
                    grid, r + 1, dirs[4].first, c + 1, dirs[4].second, rows, cols, word) ||
                // start at TL, go SE.
                match_search(
                    grid, r - 1, dirs[7].first, c - 1, dirs[7].second, rows, cols, word));

            // Check NW-SE diagonal: from bottom-left toward top-right (↗) or vice versa (↙).
            const auto  diag2 = 
                // start at BL, go NE.
                match_search(
                    grid, r + 1, dirs[5].first, c - 1, dirs[5].second, rows, cols, word) ||
                // start at TR, go SW.
                match_search(
                    grid, r - 1, dirs[6].first, c + 1, dirs[6].second, rows, cols, word); 

            if (diag1 && diag2) {
                ++count;
            } 
        }
    }
 
    return count;
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input-file>.\n";
        return 1;
    }

    std::vector<std::string> grid;
    if (!parse_input(argv[1], grid)) {
        std::cerr << "Error: could not read or parse '" << argv[1] << "'.\n";
        return 1;
    }

    if (grid.empty()) {
      std::cerr << "Grid is empty.\n";
      return 1;  
    }

    const auto row_sz {grid.size()};
    const auto col_sz {grid[0].size()};

    // Directions
    const std::vector<std::pair<int,int>> dirs = {
        {-1,  0 }, { 1,  0 },  // North (N), South (S)
        { 0, -1 }, { 0,  1 },  // West (W), East (E)
        {-1, -1 }, {-1,  1 },  // North West (NW), North East (NE)
        { 1, -1 }, { 1,  1 }   // South West (SW), South East (SE)
    };

    // Part 1
    constexpr std::string_view xmas_word {"XMAS"};
    const auto xmas_count {count_occurrences(grid, xmas_word, dirs)};
   
    // Part 2
    constexpr std::string_view x_shaped_mas_word {"MAS"};
    const auto x_shaped_mas_count {count_xmas_shapes(grid, x_shaped_mas_word, dirs)};
 
    std::cout << "Result Part 1 (XMAS word count): " << xmas_count << "\n";
    std::cout << "Result Part 2 (MAS word count): "  << x_shaped_mas_count << "\n";

    return 1;
}