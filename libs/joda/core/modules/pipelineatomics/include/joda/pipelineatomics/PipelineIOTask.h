#ifndef JODA_PIPELINE_IO_TASK_H
#define JODA_PIPELINE_IO_TASK_H

#include <joda/misc/RJFwd.h>
#include <joda/misc/RecurringTimer.h>

#include <memory>

#include "PipelineTask.h"
#include "TaskID.h"
#include "queue/PipelineIOQueue.h"
#include "queue/PipelineQueue.h"

namespace joda::queryexecution::pipeline::tasks {

template <class T>
concept Executor = requires {
  typename T::Input;                                      // Input  Type
  typename T::InputQueue;                                 // Input Queue Type
  typename T::Output;                                     // Output  Type
  typename T::OutputQueue;                                // Output Queue Type
  { ID<T>::NAME } -> std::convertible_to<TaskID>;         // Registered
  { T::InputBulk } -> std::convertible_to<size_t>;        // Input Bulk
  { T::OutputBulk } -> std::convertible_to<size_t>;       // Output Bulk
  { T::Async } -> std::convertible_to<tasks::AsyncType>;  // Async Type
};

template <Executor E>
class PipelineIOTask : public PipelineTask {
 public:
  // Input type of the task
  typedef typename E::Input Input;
  typedef typename E::InputQueue InputQueue;

  // Output type of the task
  typedef typename E::Output Output;
  typedef typename E::OutputQueue OutputQueue;

  template <typename... Args>
  PipelineIOTask(Args&&... args) : exec(std::forward<Args>(args)...) {}

  virtual ~PipelineIOTask() {
    if (!finished && output_q != nullptr) {
      // Unregister if not done before
      output_q->unregisterProducer();
    }
  };
  /**
   * @brief Executes the pipeline task by optionally reading from the input
   * queue and writing to the output queue
   *
   */
  TaskStatus execute(std::function<void(TaskStatus)> callback) override {
    DCHECK(!templateTask) << "Template tasks may not be executed";

    // Execute Task
    auto exec_status = exec.execute(input_q, output_q);

    finished = exec_status == TaskStatus::FINISHED;
    if (output_q != nullptr && finished) {
      output_q->unregisterProducer();
    }
    callback(exec_status);
    return exec_status;
  }

  virtual std::string getName() const override { return tasks::ID<E>::NAME; }

  virtual TaskNum getNum() const override { return _num; }

  virtual void setNum(TaskNum num) override { _num = num; };

  virtual AsyncType getAsyncType() const override { return E::Async; };

  virtual bool inputConnected() const override {
    return std::is_same<Input, pipeline::NoOpt>::value || input_q != nullptr;
  };

  virtual bool outputConnected() const override {
    return std::is_same<Output, pipeline::NoOpt>::value || output_q != nullptr;
  };

  virtual PipelineQueuePtr createInputQueue(size_t max_threads) override {
    auto capacity = E::InputBulk * (max_threads + 1);
    auto queue = std::make_unique<InputQueue>(getNum(), capacity);
    input_q = queue.get();
    return queue;
  };

  virtual void setInputQueue(PipelineQueue* queue) override {
    if (queue == nullptr) return;
    DCHECK_EQ(output_q, nullptr) << "Input queue already set";
    auto* cast_queue = dynamic_cast<InputQueue*>(queue);
    DCHECK_NE(cast_queue, nullptr) << "Queue is not of expected type";
    input_q = cast_queue;
  }

  virtual void setOutputQueue(PipelineQueue* queue) override {
    if (queue == nullptr) return;
    DCHECK_EQ(output_q, nullptr) << "Output queue already set";
    auto* cast_queue = dynamic_cast<OutputQueue*>(queue);
    DCHECK_NE(cast_queue, nullptr) << "Queue is not of expected type";
    output_q = cast_queue;
    output_q->registerProducer();
  };

  virtual bool isFinished() const override { return finished; }

  virtual PipelineTaskPtr clone() override {
    DCHECK(templateTask) << "Only template tasks may be cloned";
    auto clone = std::make_unique<PipelineIOTask<E>>(exec);
    clone->templateTask = false;
    clone->setOutputQueue(output_q);
    clone->input_q = input_q;
    clone->_num = _num;

    if (!finished && output_q != nullptr) {
      // Template instantiated, "reserving" producer not required anymore
      output_q->unregisterProducer();
      finished = true;  // use finished status as a flag to indicate that the
                        // task removed the "reserving" producer from the queue
    }

    return std::move(clone);
  };

  virtual RJValue toJSON(RJMemoryPoolAlloc& alloc) const override {
    RJValue obj(rapidjson::kObjectType);
    obj.AddMember("Num", getNum(), alloc);
    obj.AddMember("Name", std::string(tasks::ID<E>::NAME), alloc);
    switch (E::Async) {
      case tasks::AsyncType::Synchronous:
        obj.AddMember("Async", "Synchronous", alloc);
        break;
      case tasks::AsyncType::SingleThreaded:
        obj.AddMember("Async", "SingleThreaded", alloc);
        break;
      case tasks::AsyncType::MultiThreaded:
        obj.AddMember("Async", "MultiThreaded", alloc);
        break;
      default:
        obj.AddMember("Async", "Unknown", alloc);
        break;
    }

    auto desc = std::string(exec.toString());
    if (desc != "") {
      obj.AddMember("Description", desc, alloc);
    }

    return obj;
  };

  TaskStatistics getStatistics() const override {
    TaskStatistics stats;

    auto execTime = getExecutionTime();
    stats.runtime = execTime.getDuration();
    stats.max_runtime = stats.runtime;
    stats.min_runtime = stats.runtime;
    return stats;
  }

  auto getData() const { return exec.getData(); }
  auto getDesc() const { return exec.toString(); }
  auto getExecutionTime() const { return exec.getTimer(); }

 private:
  TaskNum _num = PipelineTask::UNSET_NUM;
  InputQueue* input_q = nullptr;
  OutputQueue* output_q = nullptr;
  E exec;
  bool finished = false;
  bool templateTask = true;
};

}  // namespace joda::queryexecution::pipeline::tasks
#endif  // JODA_PIPELINE_IO_TASK_H