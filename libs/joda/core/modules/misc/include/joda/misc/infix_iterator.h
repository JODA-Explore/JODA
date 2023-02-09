//
// Created by Nico Schäfer on 11/2/17.
//

#ifndef JODA_INFIX_ITERATOR_H
#define JODA_INFIX_ITERATOR_H
#include <iterator>
#include <ostream>
template <class T, class charT = char, class traits = std::char_traits<charT> >
class infix_ostream_iterator {
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = void;
  using pointer = void;
  using reference = void;

  std::basic_ostream<charT, traits> *os;
  charT const *delimiter;
  bool first_elem;

 public:
  typedef charT char_type;
  typedef traits traits_type;
  typedef std::basic_ostream<charT, traits> ostream_type;
  infix_ostream_iterator(ostream_type &s)
      : os(&s), delimiter(0), first_elem(true) {}
  infix_ostream_iterator(ostream_type &s, charT const *d)
      : os(&s), delimiter(d), first_elem(true) {}
  infix_ostream_iterator<T, charT, traits> &operator=(T const &item) {
    // Here's the only real change from ostream_iterator:
    // Normally, the '*os << item;' would come before the 'if'.
    if (!first_elem && delimiter != 0) *os << delimiter;
    *os << item;
    first_elem = false;
    return *this;
  }
  infix_ostream_iterator<T, charT, traits> &operator*() { return *this; }
  infix_ostream_iterator<T, charT, traits> &operator++() { return *this; }
  infix_ostream_iterator<T, charT, traits> &operator++(int) { return *this; }
};
#endif  // JODA_INFIX_ITERATOR_H
