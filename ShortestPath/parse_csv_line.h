#include <string>
#include <exception>
#include <vector>

struct unterminated_quote_exception : public std::exception {
   const char * what () const throw () {
      return "Invalid Line encountered in CSV: Unterminated quotes";
   }
};

extern std::vector<std::string> parse_quoted_csv(std::string input_line);  // Throws unterminated_quote_exception
