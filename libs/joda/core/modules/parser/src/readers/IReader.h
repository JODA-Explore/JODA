//
// Created by Nico on 27/11/2018.
//

#ifndef JODA_IREADER_H
#define JODA_IREADER_H

#include <joda/parser/ReaderFlags.h>
#include <joda/parser/ParserFlags.h>
#include "../../../concurrency/include/joda/concurrency/IThreadUser.h"



 /**
  * Interface for classes reading documents and commiting their contents into a queue
  * The reader and parser flags describe which kind of data can be read and what is returned.
  * @tparam R_FLAGS Flags representing the source from which this class reads
  * @tparam P_FLAGS Flags representing the output of this reader
  */
template <ReaderFlag R_FLAGS,ParserFlag P_FLAGS >
class IReader : public IThreadUser {
 public:
  /*
   * Types
   */
  typedef typename JODA_READER_QUEUE<R_FLAGS>::queue_t RQueue;
  typedef typename JODA_PARSER_QUEUE<P_FLAGS>::queue_t PQueue;
  typedef typename JODA_READER_QUEUE<R_FLAGS>::payload_t RPayload;
  typedef typename JODA_PARSER_QUEUE<P_FLAGS>::payload_t PPayload;
  typedef JODA_READER_QUEUE<R_FLAGS> RQueueFactory;
  typedef JODA_PARSER_QUEUE<P_FLAGS> PQueueFactory;
  /*
   * Constructor/Destructor
   */
  /**
   * Creates a new reader with the given input and output queue
   * @param rqueue Input queue
   * @param pqueue Output queue
   */
  IReader(std::unique_ptr<RQueue>& rqueue,std::unique_ptr<PQueue> & pqueue);
  /**
   * Creates a new reader with the given input and output queue and a maximum number of threads to use during reading
   * @param rqueue Input queue
   * @param pqueue Output queue
   * @param maxThreads maximum number of threads to use during reading
   */
  IReader(std::unique_ptr<RQueue>& rqueue,std::unique_ptr<PQueue> & pqueue, unsigned int maxThreads );
  virtual ~IReader() = default;


  /*
   * Read
   */
  /**
   * Reads everything that is passed through the queue, and outputs it into the output queue
   */
  virtual void read() { read(1); }

  /**
   * Reads a sample of everything that is passed through the queue, and outputs it into the output queue
   * @param sample The sample size to take in [0,1]
   */
  virtual void read(double sample) = 0;

  /**
   * Checks whether the reader is finished.
   * @return
   */
  virtual bool finishedReading() const = 0;


 protected:
  std::unique_ptr<RQueue>& rqueue;
  std::unique_ptr<PQueue>& pqueue;

 public:


};

template<ReaderFlag R_FLAGS, ParserFlag P_FLAGS>
IReader<R_FLAGS, P_FLAGS>::IReader(std::unique_ptr<IReader::RQueue> &rqueue,
                                   std::unique_ptr<IReader::PQueue> &pqueue) : IReader(rqueue, pqueue, 1)
{
}

template<ReaderFlag R_FLAGS, ParserFlag P_FLAGS>
IReader<R_FLAGS, P_FLAGS>::IReader(std::unique_ptr<IReader::RQueue> &rqueue,
                                   std::unique_ptr<IReader::PQueue> &pqueue,
                                   unsigned int maxThreads):IThreadUser(maxThreads), rqueue(rqueue), pqueue(pqueue)
                                   {
                                     DLOG(INFO)
                                     << "Started IReader<" << JODA_FLAG_T(R_FLAGS) << "," << JODA_FLAG_T(P_FLAGS)
                                     << "> with " << maxThreads << " threads";
}

#endif //JODA_IREADER_H
