
import csv

# Initializes the state, given a parameter string
def init(param):
  csvfile = open(param, 'w', newline='')
  return (csvfile, None)

# Exports a single document, given the staten and the document itself
def set_next(state, doc):
  if state[1] is None:
    attributes = doc.keys()
    writer = csv.DictWriter(state[0], fieldnames=attributes, delimiter=',')
    writer.writeheader()
    state = (state[0], writer)
  state[1].writerow(doc)

def finalize(state):
  state[0].close()
