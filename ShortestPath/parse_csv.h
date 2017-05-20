#include <string>
#include <exception>
#include <vector>
#include <unordered_map>

struct unterminated_quote_exception : public std::exception {
   const char * what () const throw () {
      return "Invalid Line encountered in CSV: Unterminated quotes";
   }
};

struct missing_file_exception : public std::exception {
   const char * what () const throw () {
      return "Unable to locate necessary input file.";
   }
};

extern std::vector<std::string> parse_quoted_csv_line(std::string input_line);  // Throws unterminated_quote_exception

extern const bool parse_quoted_csv_file(std::string folder, std::string filename,
    std::vector<std::unordered_map<std::string, std::string>> &result); // Throws unterminated_quote_exception and missing_file_exception
