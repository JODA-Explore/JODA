# Number of arguments for main function (aggregate)
num_args = 1

# Aggregate function, takes a state and the specified number of arguments and returns a new state
def aggregate(state, arg1):
  return state

# Merges the own state with another state
def merge(state, other):
  return state

# Returns the aggregation result based on the state
def finalize(state):
  return 1

# Initial state
def init_state():
  return 1