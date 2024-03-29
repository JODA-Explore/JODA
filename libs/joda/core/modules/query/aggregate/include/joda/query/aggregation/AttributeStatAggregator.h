//
// Created by Nico Schäfer on 10/31/17.
//

#ifndef JODA_ATTRIBUTESTATAGGREGATOR_H
#define JODA_ATTRIBUTESTATAGGREGATOR_H

#include <map>

#include "IAggregator.h"
#include "joda/misc/RJFwd.h"
namespace joda::query {
class AttributeStatAggregator : public IAggregator {
 public:
  AttributeStatAggregator(
      const std::string &toPointer,
      std::vector<std::unique_ptr<IValueProvider>> &&params);
  void merge(IAggregator *other) override;
  RJValue terminate(RJMemoryPoolAlloc &alloc) override;
  std::unique_ptr<IAggregator> duplicate() const override;
  void accumulate(const RapidJsonDocument &json,
                  RJMemoryPoolAlloc &alloc) override;
  const std::string getName() const override;
  std::vector<std::string> getAttributes() const override;

  static constexpr auto getName_() { return "ATTSTAT"; }

 protected:
  class AttStatHandler {
   public:
    struct Node {
      Node(const std::string &name, Node *parent) : name(name), parent(parent) {
        if (this->parent != nullptr) {
          DCHECK(!this->name.empty());
          DCHECK(this->parent != this);
        }
      }

      Node(const Node &n) = delete;
      Node &operator=(const Node &n) = delete;
      Node(Node &&n) = default;
      Node &operator=(Node &&n) = default;

      std::string name;
      size_t count_obj = 0;
      uint64_t max_member = 0;
      uint64_t min_member = std::numeric_limits<uint64_t>::max();
      size_t count_arr = 0;
      uint64_t max_size = 0;
      uint64_t min_size = std::numeric_limits<uint64_t>::max();
      size_t count_null = 0;
      size_t count_str = 0;
      uint64_t max_strsize = 0;
      uint64_t min_strsize = std::numeric_limits<uint64_t>::max();
      size_t count_double = 0;
      double max_double = std::numeric_limits<double>::lowest();
      double min_double = std::numeric_limits<double>::max();
      size_t count_int = 0;
      int64_t max_int = std::numeric_limits<int64_t>::min();
      int64_t min_int = std::numeric_limits<int64_t>::max();
      size_t count_bool = 0;
      size_t count_true = 0;
      size_t count_false = 0;

      size_t getCount() const {
        return count_null + count_obj + count_arr + count_int + count_double +
               count_bool + count_str;
      }

      Node *findChild(const std::string &name) {
        DCHECK(count_obj > 0);
        Node *c = nullptr;
        for (auto &child : children) {
          if (child.name == name) {
            c = &child;
            break;
          }
        }
        if (c == nullptr) {
          Node n(name, this);
          children.emplace_back(std::move(n));
          DCHECK(!children.empty());
          c = &children.back();
        }
        DCHECK(c != nullptr);
        c->parent = this;  // Update, if pointer changed
        return c;
      }

      Node *findArrElt(size_t i) {
        DCHECK(count_arr > 0);
        Node *c = nullptr;
        if (arrMembers.size() > i) {
          c = &arrMembers[i];
        } else {
          Node n(std::to_string(i), this);
          arrMembers.emplace_back(std::move(n));
          DCHECK(arrMembers.size() == i + 1);
          c = &arrMembers.back();
        }
        DCHECK(c != nullptr);
        c->parent = this;  // Update, if pointer changed
        return c;
      }

      void merge(const Node *o) {
        if (o == nullptr) return;
        DCHECK(o->name == name);
        count_obj += o->count_obj;
        count_arr += o->count_arr;
        count_int += o->count_int;
        count_double += o->count_double;
        count_str += o->count_str;
        count_null += o->count_null;
        count_bool += o->count_bool;
        count_true += o->count_true;
        count_false += o->count_false;
        min_int = std::min(min_int, o->min_int);
        max_int = std::max(max_int, o->max_int);
        min_double = std::min(min_double, o->min_double);
        max_double = std::max(max_double, o->max_double);
        min_member = std::min(min_member, o->min_member);
        max_member = std::max(max_member, o->max_member);
        min_size = std::min(min_size, o->min_size);
        max_size = std::max(max_size, o->max_size);
        min_strsize = std::min(min_strsize, o->min_strsize);
        max_strsize = std::max(max_strsize, o->max_strsize);
        for (const auto &child : o->children) {
          auto c = findChild(child.name);
          c->merge(&child);
        }
        for (size_t i = 0; i < o->arrMembers.size(); ++i) {
          auto c = findArrElt(i);
          c->merge(&o->arrMembers[i]);
        }
      }

      RJValue toValue(RJMemoryPoolAlloc &alloc) const {
        RJValue r(rapidjson::kObjectType);
        if (!name.empty()) {
          r.AddMember("Key", name, alloc);
        }
        r.AddMember("Count_Total", getCount(), alloc);
        r.AddMember("Count_Object", count_obj, alloc);
        if (min_member < std::numeric_limits<uint64_t>::max()) {
          r.AddMember("Min_Member", min_member, alloc);
          r.AddMember("Max_Member", max_member, alloc);
        }
        r.AddMember("Count_Array", count_arr, alloc);
        if (min_size < std::numeric_limits<uint64_t>::max()) {
          r.AddMember("Min_Size", min_size, alloc);
          r.AddMember("Max_Size", max_size, alloc);
        }
        r.AddMember("Count_Null", count_null, alloc);
        r.AddMember("Count_Boolean", count_bool, alloc);
        r.AddMember("Count_True", count_true, alloc);
        r.AddMember("Count_False", count_false, alloc);
        r.AddMember("Count_String", count_str, alloc);
        if (min_strsize < std::numeric_limits<uint64_t>::max()) {
          r.AddMember("Min_StrSize", min_strsize, alloc);
          r.AddMember("Max_StrSize", max_strsize, alloc);
        }
        r.AddMember("Count_Int", count_int, alloc);
        if (count_int > 0) {
          r.AddMember("Min_Int", min_int, alloc);
          r.AddMember("Max_Int", max_int, alloc);
        }
        r.AddMember("Count_Float", count_double, alloc);
        if (count_double > 0) {
          r.AddMember("Min_Float", min_double, alloc);
          r.AddMember("Max_Float", max_double, alloc);
        }
        r.AddMember("Count_Number", count_int + count_double, alloc);
        if (!children.empty()) {
          RJValue ch(rapidjson::kArrayType);
          for (const auto &child : children) {
            auto child_val = child.toValue(alloc);
            ch.PushBack(child_val, alloc);
          }
          r.AddMember("Children", ch, alloc);
        }
        if (!arrMembers.empty()) {
          RJValue ch(rapidjson::kArrayType);
          for (const auto &child : arrMembers) {
            auto child_val = child.toValue(alloc);
            ch.PushBack(child_val, alloc);
          }
          r.AddMember("Array_Items", ch, alloc);
        }
        return r;
      }

      std::vector<Node> children{};
      std::vector<Node> arrMembers{};
      Node *parent = nullptr;
    };

    AttStatHandler() { currnode = &root; }

    typedef char Ch;

    void merge(const Node &n) { root.merge(&n); }

    void finishValue() {
      DCHECK(currnode != nullptr);
      currnode = currnode->parent;
    }

    bool Null() {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_null++;
      finishValue();
      return true;
    }

    bool Bool(bool b) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_bool++;
      if (b) {
        currnode->count_true++;
      } else {
        currnode->count_false++;
      }
      finishValue();
      return true;
    }

    bool Int(int i) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_int++;
      currnode->min_int = std::min((int64_t)i, currnode->min_int);
      currnode->max_int = std::max((int64_t)i, currnode->max_int);
      finishValue();
      return true;
    }

    bool Uint(unsigned i) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_int++;
      currnode->min_int = std::min((int64_t)i, currnode->min_int);
      currnode->max_int = std::max((int64_t)i, currnode->max_int);
      finishValue();
      return true;
    }

    bool Int64(int64_t i) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_int++;
      currnode->min_int = std::min(i, currnode->min_int);
      currnode->max_int = std::max(i, currnode->max_int);
      finishValue();
      return true;
    }

    bool Uint64(uint64_t i) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_int++;
      currnode->min_int = std::min((int64_t)i, currnode->min_int);
      currnode->max_int = std::max((int64_t)i, currnode->max_int);
      finishValue();
      return true;
    }

    bool Double(double d) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_double++;
      currnode->min_double = std::min(d, currnode->min_double);
      currnode->max_double = std::max(d, currnode->max_double);
      finishValue();
      return true;
    }

    bool RawNumber(const Ch *str, rapidjson::SizeType length, bool copy) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_double++;
      finishValue();
      return true;
    }

    bool String(const Ch *str, rapidjson::SizeType length, bool copy) {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_str++;
      currnode->min_strsize =
          std::min(static_cast<u_int64_t>(length), currnode->min_strsize);
      currnode->max_strsize =
          std::max(static_cast<u_int64_t>(length), currnode->max_strsize);
      finishValue();
      return true;
    }

    bool StartObject() {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_obj++;
      levels.emplace_back(false);
      return true;
    }

    bool Key(const Ch *str, rapidjson::SizeType length, bool copy) {
      auto name = std::string(str, length);
      DCHECK(!name.empty());
      DCHECK(currnode != nullptr);
      currnode = currnode->findChild(name);
      return true;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
      DCHECK(currnode != nullptr);
      currnode->min_member =
          std::min(static_cast<u_int64_t>(memberCount), currnode->min_member);
      currnode->max_member =
          std::max(static_cast<u_int64_t>(memberCount), currnode->max_member);
      finishValue();
      levels.pop_back();
      return true;
    }

    bool StartArray() {
      checkArray();
      DCHECK(currnode != nullptr);
      currnode->count_arr++;
      levels.emplace_back(true);
      return true;
    }

    bool EndArray(rapidjson::SizeType elementCount) {
      DCHECK(currnode != nullptr);
      currnode->min_size =
          std::min(static_cast<u_int64_t>(elementCount), currnode->min_size);
      currnode->max_size =
          std::max(static_cast<u_int64_t>(elementCount), currnode->max_size);
      finishValue();
      levels.pop_back();
      return true;
    }

    void reset() {
      root = {"", nullptr};
      currnode = &root;
      levels.clear();
    }

    Node finish() { return std::move(root); }

    void finishDocument() {
      currnode = &root;
      levels.clear();
    }

   private:
    struct Level {
      Level(bool isArr) : isArr(isArr) {}

      bool isArr;
      size_t count = 0;
    };

    void checkArray() {
      if (levels.empty()) return;
      if (levels.back().isArr) {
        DCHECK(currnode != nullptr);
        currnode = currnode->findArrElt(levels.back().count);
        levels.back().count++;
      }
    }

    Node root{"", nullptr};
    Node *currnode = nullptr;
    std::vector<Level> levels{};
  };

  AttStatHandler handler{};
};
}  // namespace joda::query

#endif  // JODA_ATTRIBUTESTATAGGREGATOR_H
