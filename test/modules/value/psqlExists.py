import psycopg2
from psycopg2 import Error



# Connection cache
con_cache = None
con_cache_string = None

def connect_if_needed(con_string):
  global con_cache_string
  global con_cache
  if con_cache_string is None or con_string != con_cache_string or not con_cache:
    if(con_cache):
      con_cache.close()
    try:
      # Connect to an existing database
      con_cache = psycopg2.connect(con_string)
      con_cache_string = con_string

    except (Exception, Error) as error:
        print("Error while connecting to PostgreSQL", error)


# Number of arguments for main function (get_value)
num_args = 4

# Given a number of arguments, returns an arbitrary value
def get_value(con_string, table, attribute, value):
  connect_if_needed(con_string)
  try:
      # Create a cursor to perform database operations
      cursor = con_cache.cursor()
      # Executing a SQL query
      query = "SELECT * FROM " + table + " WHERE " + attribute + " = %s LIMIT 1"
      cursor.execute(query,(value,))
      print(cursor.query)
      print(str(cursor.rowcount))
      # Fetch result
      if cursor.rowcount > 0:
        print("Return true")
        return True

  except (Exception, Error) as error:
      print("Error while connecting to PostgreSQL", error)
  finally:
      if (con_cache):
          cursor.close()
  print("Return false")
  return False
