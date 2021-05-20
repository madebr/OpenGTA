/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/
#include "pf_tree.hpp"

#include "m_exceptions.h"

#include <cstring>
#include <string>

namespace PrefixFreeTree {

  Node::~Node() {
    for (MapType::iterator i = map.begin(); i != map.end(); i++) {
      delete i->second;
    }
    map.clear();
  }

  Node & Node::insert(const char * str, size_t offset) {
    size_t str_len = strlen(str);
    if (offset < str_len) {
      value_t node_val = str[offset];
      MapType::iterator i = map.find(node_val);
      offset++;
      if (i == map.end()) {
        map[node_val] = new Node();
        return map[node_val]->insert(str, offset);
      }
      else {
        if (i->second->isLeaf())
            throw E_INVALIDFORMAT(std::string { "Cannot enter '" } + str
                                  + "' at offset " + std::to_string(offset)
                                  + " as a leaf node already exists");
        if ((offset == str_len) && (!i->second->isLeaf()))
          throw E_INVALIDFORMAT(std::string { "Cannot enter '" } + str
                                + "' as a non-leaf node already exists");
        return i->second->insert(str, offset);
      }
    }
    return *this;
  }

}

#ifdef SIMPLE_TREE_DEMO
#include <iostream>

template <typename T>
struct Echo {
  void call(T t) {
    std::cout << "Echo: " << t << std::endl;
  }
};

using namespace std;
int main(int argc, char* argv[]) {
  PrefixFreeTree::Walker<uint32_t, Echo> m;
  uint32_t i = 1;
  while (!cin.eof()) {
    std::string inputs;
    cin >> inputs;
    if (inputs.size()) {
      std::cout << "-" << inputs << "-" << std::endl;
      PrefixFreeTree::Node & n = m.insert(inputs.c_str());
      n.leaf_value = i;
      i++;
    }
  }

  const char* input = argv[1];
  int len = strlen(input);
  for (int i = 0; i < len; i++) {
    m.push(input[i]);
  }
}

#endif
