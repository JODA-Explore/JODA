# Number of arguments for main function (aggregate)
num_args = 1

def calculate_bbox(box1, box2):
  if box1[0] is None or box2[0] < box1[0]:
    box1[0] = box2[0]
  if box1[1] is None or box2[1] < box1[1]:
    box1[1] = box2[1]
  if box1[2] is None or box2[2] > box1[2]:
    box1[2] = box2[2]
  if box1[3] is None or box2[3] > box1[3]:
    box1[3] = box2[3]
  return box1

# Aggregate function, takes a state and the specified number of arguments and returns a new state
def aggregate(state, coord):
  box = [coord[0], coord[1], coord[0], coord[1]]
  state = calculate_bbox(state, box)
  return state

# Merges the own state with another state
def merge(state, other):
  state = calculate_bbox(state, other)
  return state

# Returns the aggregation result based on the state
def finalize(state):
  return state

# Initial state
def init_state():
  return [None, None, None, None]







