
# Initializes the state, given a parameter string
def init(param):
  return "Prefix"

# Exports a single document, given the staten and the document itself
def set_next(state, doc):
  print(state + str(doc))

# Is called after the export of all documents is finished to perform cleanup
def finalize(state):
  pass

