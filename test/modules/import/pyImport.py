
# Initializes the state, given a parameter string
def init(param):
  return ["Doc1", "Doc2", "Doc3"]

# Returns the next document from the input, given the state
# If no more documents are available, returns None.
def get_next(state):
  if len(state) == 0:
    return None
  else:
    return state.pop()

