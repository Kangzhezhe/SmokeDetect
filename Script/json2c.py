import json
import os
os.chdir(os.path.dirname(__file__))
import re

def get_dimension(lst):
    if isinstance(lst, list):
        return 1 + get_dimension(lst[0])
    else:
        return 0

def repalce_name(var_name):
    return re.sub(r'[^a-zA-Z0-9]', '_', var_name)

# Load the model parameters from the JSON file
with open('model_parameters.json', 'r') as f:
    model_params = json.load(f)
c_array_str = ""

for key in model_params.keys():
    # Convert parameters to a C language array format
    # import ipdb;ipdb.set_trace()
    if get_dimension(model_params[key]) == 1:
        c_array_str += "float "+repalce_name(key)+"["+ str(len(model_params[key]))+"] = {"
        c_array_str += ", ".join([str(val) for val in model_params[key]])
        c_array_str += "};"
    else :
        c_array_str += "float "+repalce_name(key)+"["+ str(len(model_params[key]))+"]["+ str(len(model_params[key][0])) + "] = {"
        for sublist in model_params[key]:
            c_array_str += "{" + ", ".join([str(val) for val in sublist]) + "},\n"
        c_array_str = c_array_str[:-2]  # Remove the extra comma and newline
        c_array_str += "};"
    c_array_str += "\n"

# Write the C array to a file
with open('model_parameters.c', 'w') as f:
    f.write(c_array_str)