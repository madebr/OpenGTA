#include "datahelper.h"
#include "opengta.h"
#include "cistring.h"
#include "m_exceptions.h"
#include "log.h"

namespace OpenGTA {
  namespace Helper {
    // level-filename to index-into-message-db
    // needed for area-name lookup
    size_t mapFileName2Number(const std::string & file) {
      size_t num = 0;
      Util::ci_string ci_file(file.c_str());
#define STRING_TEST(n) ci_file.find(n) != std::string::npos
      if (STRING_TEST("nyc.cmp"))
        num = 1;
      else if (STRING_TEST("sanb.cmp"))
        num = 2;
      else if (STRING_TEST("miami.cmp"))
        num = 3;
      else
        ERROR << "unknown level: " << file << std::endl;
#undef STRING_TEST
      return num;
    }
  }
}
