# Number of arguments for main function (aggregate)
num_args = 2

# Aggregate function, takes a state and the specified number of arguments and returns a new state
def aggregate(state: set, arg1, arg2):
  if(not isinstance(arg1, str) or not isinstance(arg2, str)):
    return state
  
  state.add(arg1[0]+arg2[0])
  return state

# Merges the own state with another state
def merge(state, other):
  state.update(other)
  return state

# Returns the aggregation result based on the state
def finalize(state):
  return list(state)

# Initial state
def init_state():
  return set()
