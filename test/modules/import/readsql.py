import psycopg2
from psycopg2 import Error
import psycopg2.extras

# Initializes the state, given a parameter string
# Expects "<PSQL connection string> <table>" as config string
def init(param):
  params = param.rsplit(" ", 1)
  conn = psycopg2.connect(params[0])
  conn.set_client_encoding('UTF8')
  table = params[1]
  try:
      # Create a cursor to perform database operations
      cursor = conn.cursor(cursor_factory=psycopg2.extras.DictCursor)
      # Executing a SQL query
      query = "SELECT * FROM " + table
      cursor.execute(query)
      # Fetch result
      if cursor.rowcount > 0:
        return (conn,cursor)

  except (Exception, Error) as error:
      print(error)
      return None
  return None

# Returns the next document from the input, given the state
# If no more documents are available, returns None.
def get_next(state):
  if state is None:
    return None
  else:
    conn,cursor = state
    try:
      # Fetch result
      res = cursor.fetchone()
      if res is None:
        conn.close()
        return None
      else:
        res_dict = dict(res)
        return res_dict
    except (Exception, Error) as error:
      print(error)
      conn.close()
      return None
  

