//
// Created by Nico on 13/05/2019.
//

#ifndef JODA_DEFAULTCONTAINERSCHEDULER_H
#define JODA_DEFAULTCONTAINERSCHEDULER_H

#include <joda/container/ContainerFlags.h>
#include <joda/container/JSONContainer.h>
#include <joda/misc/RJFwd.h>
#include <rapidjson/istreamwrapper.h>

namespace joda::docparsing {
/**
 * A document scheduler scheduling the documents into the same container
 * @tparam meta If the Container should be meta finalized or really finalized
 */
template <bool meta>
class DefaultContainerScheduler {
 public:
  typedef short ContainerIdentifier;

  /**
   * Constructs a scheduler for use within the parsers.
   * @param queue  The container queue to use to pass finalized containers
   * through.
   * @param contSize The size of newly constructed containers.
   */
  DefaultContainerScheduler(JsonContainerQueue::queue_t *queue,
                            size_t contSize = 0);

  /**
   * Always returns 0, as this scheduler does not require an id
   * @param doc The document
   * @return 0
   */
  ContainerIdentifier getContainerForDoc(const RJDocument &doc);

  /**
   * Always returns 0, as this scheduler does not require an id
   * @param raw The document in string representation (before parsing)
   * @return 0
   */
  ContainerIdentifier getContainerForDoc(std::string &raw);

  /**
   * Always returns 0, as this scheduler does not require an id
   * @param stream The stream to check against
   * @return 0
   */
  ContainerIdentifier getContainerForDoc(rapidjson::IStreamWrapper &stream);

  /**
   * Schedules the document into the container given by id.
   * @param id The id of the container
   * @param doc The document to schedule
   * @param origin The origin of the document
   * @param size The size of the document (bytes)
   */
  void scheduleDocument(ContainerIdentifier id,
                        std::unique_ptr<RJDocument> &&doc,
                        std::unique_ptr<IOrigin> &&origin, size_t size);

  /**
   * Returns a document initialized with the allocator of the container
   * represented by id
   * @param id The id representing the container
   * @return A new document
   */
  std::unique_ptr<RJDocument> getNewDoc(ContainerIdentifier id);

  /**
   * Finalizes the Scheduler.
   * Has to be called before deconstructing it.
   */
  void finalize();

  virtual ~DefaultContainerScheduler() = default;

 private:
  JsonContainerQueue::queue_t *queue;
  size_t contSize;

  std::unique_ptr<JSONContainer> currentContainer;
  std::unique_ptr<JSONContainer> createContainer(size_t contSize) const;
};
}  // namespace joda::docparsing

#endif  // JODA_DEFAULTCONTAINERSCHEDULER_H
