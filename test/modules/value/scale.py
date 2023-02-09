scale = None

# Number of arguments for main function (get_value)
num_args = 1

# Given a number of arguments, returns an arbitrary value
def get_value(arg1):
  global scale
  if isinstance(arg1,dict) and "0" in arg1:
    scale = arg1
    print("Scale set")
  if isinstance(arg1,dict) and "feature_0" in arg1:
    for i in range(10):
      i_str = str(i)
      f = "feature_"+i_str
      arg1[f] = (arg1[f]-scale[i_str]["min"]/scale[i_str]["max"]-scale[i_str]["min"])
    return arg1
  return None