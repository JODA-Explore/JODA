# Example implementation of a textual "average" sum function
# Aggregator functions are stateless and have to rely on the supplied state object.
# No external global variables can be used as this script will be called by multiple threads.

# Number of arguments for main function (aggregate)
num_args = 1

# Aggregate function, takes a state and the specified number of arguments and returns a new state
def aggregate(state, arg1):
  state[0] += arg1
  state[1] += 1
  return state

# Merges the own state with another state
def merge(state, other):
  state[0] += other[0]
  state[1] += other[1]
  return state

# Returns the aggregation result based on the state
def finalize(state):
  avg = state[0] / state[1]
  ret = f"AVG: {avg}. SUM: {state[0]}. COUNT: {state[1]}"
  return ret

# Initial state
def init_state():
  return [0, 0]
