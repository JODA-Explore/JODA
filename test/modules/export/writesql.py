import psycopg2
from psycopg2 import Error
from psycopg2.extensions import AsIs

# Initializes the state, given a parameter string
# Expects "<PSQL connection string> <table>" as config string
def init(param):
  params = param.rsplit(" ", 1)
  conn = psycopg2.connect(params[0])
  conn.set_client_encoding('UTF8')
  table = params[1]
  try:
      # Create a cursor to perform database operations
      cursor = conn.cursor()

      return (conn,cursor, table)

  except (Exception, Error) as error:
      print(error)
      return None
  return None

# Exports a single document, given the staten and the document itself
def set_next(state, doc):
  if state is None:
    return
  conn,cursor, table = state
  columns = doc.keys()
  values = [doc[column] for column in columns]
  insert_statement = 'insert into '+table+' (%s) values %s'
  
  # Executing a SQL query
  try:
    cursor.execute(insert_statement, (AsIs(','.join(columns)), tuple(values)))
  except (Exception, Error) as error:
    print(error)
    return None


# Is called after the export of all documents is finished to perform cleanup
def finalize(state):
  if state is None:
    return
  conn,cursor, table = state
  try:
    conn.commit()
    conn.close()
  except (Exception, Error) as error:
    print(error)
    conn.close()

