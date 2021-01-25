//
// Created by Nico on 29/08/2019.
//

#ifndef JODA_VIEWSTRUCTURE_H
#define JODA_VIEWSTRUCTURE_H

#include <string>
#include <unordered_map>

class VirtualObject;

class ViewStructure {

 public:
  /*
  * Generic
  */
 public:
  size_t estimateSize() const;

  /*
   * String/ID Map
   */
 public:
  typedef size_t ID;
  typedef std::string KEY;

  ViewStructure(size_t docCount = 1);
  ViewStructure(const ViewStructure &e) = delete;
  ViewStructure(ViewStructure &&e) = default;
  ViewStructure &operator=(const ViewStructure &e) = delete;
  ViewStructure &operator=(ViewStructure &&e) = default;

  ID getOrAdd(const KEY &key);
  ID getOrAdd(KEY &&key);
  const KEY *getKey(const ID &id) const;
 private:
  ID curr = 0;
  typedef std::unordered_map<KEY, ID> KEY_ID_M_T;
  typedef std::unordered_map<ID, const KEY *> ID_KEY_M_T;

  KEY_ID_M_T key_id;
  ID_KEY_M_T id_key;


  /*
   * Object member storage
   */
 public:
  std::pair<size_t, size_t> reserve(size_t size);
  void addMember(size_t i, ViewStructure::ID, const RJValue *val);
  void addMember(size_t i, ViewStructure::ID, const VirtualObject *val);

  struct ObjectMember {
    ObjectMember() : key(0), val(nullptr), obj(nullptr) {};
    ObjectMember(const ObjectMember &o) = delete;
    ObjectMember &operator=(const ObjectMember &o) = delete;
    ObjectMember(ObjectMember &&o) = default;
    ObjectMember &operator=(ObjectMember &&o) = default;

    ObjectMember(ID id, const RJValue *val);
    ObjectMember(ID id, const VirtualObject *val);
    ViewStructure::ID key;
    const RJValue *val;
    const VirtualObject *obj;
  };

  typedef std::vector<ObjectMember> MemberVector;
  typedef MemberVector::const_iterator MemberIterator;

  MemberIterator beginMember() const;
 private:
  MemberVector members;
  size_t docCount;
};

#endif //JODA_VIEWSTRUCTURE_H
