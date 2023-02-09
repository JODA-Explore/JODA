
import csv

# Initializes the state, given a parameter string
def init(param):
  csvfile = open(param, newline='')
  reader = csv.DictReader(csvfile,delimiter='|')
  return reader

# Returns the next document from the input, given the state
# If no more documents are available, returns None.
def get_next(state):
  
  return next(state, None)
