//
// Created by Nico Schäfer on 30/07/18.
//

#ifndef JODA_TEMPORARYORIGIN_H
#define JODA_TEMPORARYORIGIN_H

#include "IOrigin.h"

/**
 * Represents a document that fully originates from in-memory, like projected documents
 */
class TemporaryOrigin : public IOrigin {
 public:
  bool isReparsable() const override;
  std::unique_ptr<IOrigin> clone() const override;
  std::string toString() const override;

};

#endif //JODA_TEMPORARYORIGIN_H
