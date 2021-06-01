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
#ifndef PREFIX_FREE_TREE_H
#define PREFIX_FREE_TREE_H

#include <cstdint>
#include <cstdlib>
#include <map>

namespace PrefixFreeTree {

  struct Node {
    ~Node();
    typedef int value_t;
    uint32_t leaf_value;
    typedef std::map<value_t, Node* > MapType;
    MapType map;
    bool isLeaf() const { return (map.size() == 0); }
    Node & insert(const char * str, size_t offset = 0);
  };

  template <typename T, template <class> class Handler>
    class Walker : public Handler<T>, public Node {
      public:
        Walker() : Handler<T>(), Node(), curPos(this) {}

        void push(int v) {
          if (checkMap(curPos->map))
            return;
          if (checkMap(map))
            return;
          curPos = this;
        }

      private:
        Node * curPos;

        bool checkMap(const Node::MapType& m, int v)
        {
          auto it = m.find(v);
          if (it == m.end())
            return false;
          curPos = it->second;
          if (curPos->isLeaf())
            Handler<T>::call(curPos->leaf_value);
          return true;
        }
    };

}

#endif
