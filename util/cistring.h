#ifndef CASE_INSENSITIVE_STRING_H
#define CASE_INSENSITIVE_STRING_H

namespace Util {

  static int memicmp (const char *s, const char *t, int n) {
    int r = 0;
    while (n-- > 0 && (r = toupper (*s++) - toupper (*t++)) == 0);
    return (r);
  }

  struct ci_char_traits : public std::char_traits<char>
  {
    static bool eq( char c1, char c2)
    {
      return toupper(c1) == toupper(c2);
    }
    static bool lt( char c1, char c2)
    {
      return toupper(c1) < toupper(c2);
    }
    static int compare( const char *s1, const char *s2, size_t n)
    {
      return memicmp( s1, s2, n);     // non-standard !
    }
    static const char *find( const char *s, int n, char ch)
    {
      while ( n-- > 0  &&  toupper(*s) != toupper(ch) )
      {
        ++s;
      }
      return n > 0 ? s : 0;
    }
  };

  /** case-insensitive compare of std::string.
   *
   * Can't remember where I found this, probably derived from here
   * http://www.gotw.ca/gotw/029.htm anyway.
   */
  typedef std::basic_string<char, ci_char_traits> ci_string;
}
#endif
