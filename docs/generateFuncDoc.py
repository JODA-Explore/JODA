

import argparse
from typing import Any, Dict, List, Tuple
import yaml
import subprocess
from pathlib import Path
import re


class Parameter:
    def __init__(self, name: str, type: str, description: str, optional: bool = False):
        self.name = name
        self.type = type
        self.description = description
        self.optional = optional

    def __str__(self):
        return f"{self.name} {self.type} {self.description}"

    def toAttributeStr(self) -> str:
        if self.optional:
            return f"(<{self.name}>)"
        return f"<{self.name}>"

    def toDocu(self) -> str:
        return """| `{name}` | {type} | {optional} | {description} |""".format(
            name=self.name, type=self.type, description=self.description, optional=self.optional
        )

    def docuHeader() -> str:
        return """| Name | Type | Optional | Description |
| --- | --- | --- | --- |"""

    def fromSpec(spec: Dict[str, Any]) -> 'Parameter':
        return Parameter(spec["name"], spec["type"], spec["description"], spec["optional"])

    def toSpec(self) -> Dict[str, Any]:
        return {
            "name": self.name,
            "type": self.type,
            "description": self.description,
            "optional": self.optional
        }

    def mergeType(self, other: 'Parameter') -> 'Parameter':
        if other is None:
            return Parameter(self.name, self.type, self.description, True)
        if self.type != other.type:
            return Parameter(self.name, "Any", self.description, self.optional and other.optional)
        return self

    def mergeParameterLists(params: List['Parameter'], other: List['Parameter']) -> List['Parameter']:
        if other is None:
            return params
        p1 = []
        p2 = []
        if len(params) >= len(other):
            p1 = params
            p2 = other
        else:
            p1 = other
            p2 = params
        for i in range(len(p1)):
            if i < len(p2):
                p1[i] = p1[i].mergeType(p2[i])
            else:
                p1[i] = p1[i].mergeType(None)
        return p1


# Returns a list of all JODA functions
def getAllJODAFunctions(joda_path: str) -> List[str]:
    out = subprocess.run([joda_path, '--dump-functions'],
                         stdout=subprocess.PIPE)
    if out.returncode != 0:
        raise Exception("Failed to get list of supported functions")
    funcs = out.stdout.decode('utf-8').split(" ")
    return list(filter(lambda func: not func.isspace(), funcs))

# Update specification with previously non-existing functions


def updateSpec(functions: List[str], spec):
    for func in functions:
        if func not in spec:
            spec[func] = {
                'short-description': '',
                'description': '',
                'input-parameters': [],
                'output': '',
                'examples': []
            }
    return spec

# Creates or updates a spec file with all available functions


def createOrUpdateSpec(spec_path, joda_path):
    functions = getAllJODAFunctions(joda_path)

    # Create spec file if it does not exist
    spec_filepath = Path(spec_path)
    spec_filepath.touch(exist_ok=True)

    dataMap = {}
    # Load spec file contensts
    with open(spec_filepath) as f:
        dataMap = yaml.safe_load(f)
        if dataMap is None:
            dataMap = {}

    # Update contents with new functions
    dataMap = updateSpec(functions, dataMap)

    # Write back to spec file
    with open(spec_filepath, 'w+') as f:
        f.write(yaml.dump(dataMap))

    return dataMap

# Converts the YAML input value to a JODA string representation


def convertToJODAString(value):
    if isinstance(value, str):
        if value.startswith("'") and value.endswith("'"):
            return value
        return '"' + value + '"'
    elif value is None:
        return "null"
    elif isinstance(value, bool):
        return str(value).lower()
    else:
        return str(value)


def parseJODATypeOfString(value):
    if value.startswith("{"):
        return "Object"
    elif value.startswith("["):
        return "Array"
    elif value.startswith("\""):
        return "String"
    elif value == "true" or value == "false":
        return "Bool"
    elif value == "null":
        return "Null"
    else:
        return "Number"

# Returns the JODA/JSON type of the passed python value


def getJODATypeOfValue(value):
    if isinstance(value, bool):
        return "Bool"
    elif isinstance(value, int):
        return "Number"
    elif isinstance(value, float):
        return "Number"
    elif isinstance(value, str):
        if value.startswith("'") and value.endswith("'"):
            return "Any"
        # Check if string can be converted to float
        try:
            float(value)
            return "Number"
        except ValueError:
            return "String"
    elif isinstance(value, list):
        return "Array"
    elif isinstance(value, dict):
        return "Object"
    else:
        return "Any"

# Merges values and returns the unspecific type


def mergeTypes(values: List[str]) -> str:
    values = list(filter(lambda value: value is not None , values))
    if len(values) == 0:
        return "Any"
    elif len(values) == 1:
        return values[0]
    else:
        first = values[0]
        rest = values[1:]
        for value in rest:
            if value != first:
                return "Any"
        return first

# Returns the JODA representation of the example inputs


def getExampleInputs(example):
    if 'input' in example:
        return ", ".join([convertToJODAString(i) for i in example['input']])
    else:
        return ""

# Executes the example of a function in JODA and returns the result


def executeExample(name: str, example, joda_path: str) -> str:
    # Write example document to file
    doc_path = Path("./tmp.json")
    with open(doc_path, 'w+') as f:
        if 'document' in example:
            f.write(example['document'])
        else:
            f.write("{}")

    # Write Query
    query = "LOAD tmp FROM FILE \"{doc}\" AS ('': {name}({input})) STORE AS FILE \"out.json\"".format(
        name=name, doc=doc_path, input=getExampleInputs(example))

    # Execute query
    out = subprocess.run([joda_path, '-t', '1', '--noninteractive', "--logtostderr",
                         '--query', query], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if out.returncode != 0:
        raise Exception("Failed to execute example '{}'".format(query))

    log = out.stderr.decode('utf-8')
    log_path = Path("./log.txt")
    with open(log_path, 'w+') as f:
        f.write(log)

    # Read result
    result_path = Path("./out.json")
    with open(result_path) as f:
        result = f.read()

    # Remove temporary files
    doc_path.unlink(missing_ok=True)
    result_path.unlink(missing_ok=True)
    log_path.unlink(missing_ok=True)

    return result


# Creates the example documentation for a single example
def createExample(name: str, example, joda_path) -> Tuple[str, List[Parameter], str]:

    # Document
    doc = ''
    if 'document' in example:
        doc = """#### Input Document
```json
{document}
```

""".format(document=example['document'])

    # Execute example
    result = executeExample(name, example, joda_path)

    # Analyze parameters
    input_params = []
    if 'input' in example:
        for input in example['input']:
            input_params.append(
                Parameter("", getJODATypeOfValue(input), "", False))

    # Analyze output
    output_type = ""
    results = result.split("\n")
    filtered_results = list(filter(lambda r: not r.isspace(), results))
    if len(filtered_results) == 0:
        output_type = "None"
    else:
        output_types = list(
            map(lambda r: getJODATypeOfValue(r), filtered_results))
        output_type = mergeTypes(output_types)

    # Output
    output = """{doc}
#### Query
```joda
LOAD tmp
AS ('': {name}({input}))
```
#### Result
```json
{output}```
""".format(name=name, input=getExampleInputs(example), output=result, doc=doc)
    return output, input_params, output_type

# Creates markdown documentation file from function specification
def createDocFromSpec(name, spec, outdir, joda_path, ghpages: bool):
    # Create markdown file for function
    filepath = Path(outdir, name + ".md")
    filepath.touch(exist_ok=True)

    # Write function documentation
    with open(filepath, 'w+') as f:
        # Header
        if ghpages:
            f.write("""---
title: {name}
permalink: /functions/{low_name}
---

""".format(name=name, low_name=name.lower()))
        f.write("# " + name)
        if('category' in spec):
            f.write(" - " + spec['category'])
        f.write("\n\n")

        # Descriptions
        f.write(spec['short-description'] + "\n\n")
        if len(spec['description']) > 0:
            f.write("## Details\n\n")
            f.write(spec['description'] + "\n\n")

        # Input
        params = []
        if len(spec['input-parameters']) > 0:
            f.write("## Input parameters\n\n")
            f.write(Parameter.docuHeader() + "\n")
            for param in spec['input-parameters']:
                p = Parameter.fromSpec(param)
                f.write(p.toDocu() + "\n")
                params.append(p)
        f.write("\n")
        # Output
        if len(spec['output']) > 0:
            f.write("## Output\n\n")
            f.write(spec['output'] + "\n\n")
        
        # Examples
        f.write("## Usage\n\n")
        f.write("""```joda
{name}({inputs})
```""".format(name=name, inputs=", ".join([p.toAttributeStr() for p in params])))
        f.write("\n\n")
        if len(spec['examples']) > 0:
            f.write("## Examples\n\n")
            for i, example in enumerate(spec['examples']):
                docu, _, _ = createExample(name, example, joda_path)
                f.write("### Example {i}\n\n".format(i=i+1))
                f.write(docu + "\n\n")


def generateParameters(types: List[str]) -> List[Any]:
    if len(types) == 0:
        return []
    out = []
    for t in types:
        if t == "String":
            out.append(" 123 Test-StRiNg  ")
        elif t == "Number":
            out.append(-234.56)
        elif t == "Bool":
            out.append(True)
        elif t == "Array":
            out.append("'/arr'")
        elif t == "Object":
            out.append("''")
        else:
            out.append(None)
    return out


def testFunctionWith(types: List[str], joda_path: str, name: str):
    doc_path = Path("./tmp.json")
    params = generateParameters(types)
    inputs = ",".join([convertToJODAString(i) for i in params])
    with open(doc_path, 'w+') as f:
        f.write("{\"arr\": [1, \"test\"]}\n")
    query = "LOAD tmp FROM FILE \"{doc}\" AS ('': {name}({input})) STORE AS FILE \"./out.json\"".format(
        name=name, doc=doc_path, input=inputs)
    out = subprocess.run([joda_path, '--noninteractive', '-q', query],
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stderr = out.stderr.decode('utf-8')
    doc_path.unlink(missing_ok=True)
    if len(stderr) > 0:
        matches = re.findall(
            r"Expected (\d+) parameters, but got (\d+)", stderr, re.MULTILINE)
        if len(matches) > 0:
            match = matches[0]
            expected = int(match[0])
            got = int(match[1])
            if got < expected:
                return (None, expected, None, None)
            if got > expected:
                return (None, None, expected, None)
        matches = re.findall(r"Missing parameter (\d+)\n",
                             stderr, re.MULTILINE)
        if len(matches) > 0:
            i = int(matches[0])+1
            return (None, i, None, None)
        matches = re.findall(
            r"Parameter (\d+) is of wrong type\. Expected: (.+)\n", stderr, re.MULTILINE)
        if len(matches) > 0:
            match = matches[0]
            return (None, None, None, (int(match[0]), [match[1]]))
        matches = re.findall(
            r"Parameter (\d+) has to be constant", stderr, re.MULTILINE)
        if len(matches) > 0:
            i = int(matches[0])
            return (None, None, None, (i, ["String", "Number", "Bool"]))
        else:
            print("Unknown error: " + stderr)
    result_path = Path("./out.json")
    result = ""
    if result_path.exists():
        with open(result_path) as f:
            result = f.read()
            result = result.replace("\n", "").strip()
        result_path.unlink(missing_ok=True)
    else:
        result = "null"
    result = parseJODATypeOfString(result)
    return (result, None, None, None)


def testFunction(joda_path, name: str) -> Tuple[str, List[Parameter]]:
    print("---Testing function: " + name + "---")
    min_params = 0
    max_params = 4
    remaining_types = []
    for i in range(max_params):
        remaining_types.append(
            ["String", "Number", "Bool", "Array", "Object"])
    successful_types = []
    for t in remaining_types:
        successful_types.append([])

    finalParamaters = [None] * max_params
    result_types = []

    finished = False
    while not finished:
        params = []
        for i in range(min_params):
            if len(remaining_types[i]) == 0:
                params.append(finalParamaters[i].type)
            else:
                params.append(remaining_types[i][0])
        result_type, new_min, new_max, param_type_error = testFunctionWith(
            params, joda_path, name)
        if new_min is not None:  # Min Paramater Size Error
            min_params = new_min
            print("Truncate: Min Params: {}".format(min_params))
        elif new_max is not None:  # Max Paramater Size Error
            max_params = new_max
            print("Truncate: Max Params: {}".format(new_max))
        elif param_type_error is not None:  # Parameter Type Error
            param_index = param_type_error[0]
            param_types = param_type_error[1]
            remaining_types[param_index] = list(
                set(remaining_types[param_index]) & set(param_types))
            print("Truncate: Param {} types: {}".format(
                param_index, ",".join(remaining_types[param_index])))
        else:  # Success
            result_types.append(result_type)
            for i, p in enumerate(params):
                if p not in successful_types[i]:
                    successful_types[i].append(p)
                if p in remaining_types[i]:
                    remaining_types[i].remove(p)
            if len(params) < len(successful_types):
                successful_types[len(params)].append(None)

        for i, p in enumerate(params):
            if len(remaining_types[i]) == 0:
                if finalParamaters[i] is None:
                    optional = False
                    if None in successful_types[i]:
                        optional = True
                    finalParamaters[i] = Parameter(
                        str(i+1), mergeTypes(successful_types[i]), "", optional)
                    print("Parameter {} fixed to {}".format(
                        i, finalParamaters[i].type))

        # Increase min params if needed
        increase = True
        for i in range(min_params):
            if len(remaining_types[i]) > 0:
                increase = False
                break
        if increase:
            min_params += 1

        # Check if finished

        min_params = min(min_params, max_params)
        if min_params == max_params:
            finished = True
            for t in remaining_types[:min_params]:
                if len(t) > 0:
                    finished = False
    # Finished
    return (mergeTypes(result_types), list(filter(lambda x: x is not None, finalParamaters)))


def evaluateParameters(specs, joda_path, spec_filepath):
    for name, spec in specs.items():
        if 'input-parameters' in spec and len(spec['input-parameters']) > 0:
            continue
        if 'output' in spec and spec['output'] != "":
            continue
        out, params = testFunction(joda_path, name)
        spec['output'] = "**{}:**".format(out)
        spec['input-parameters'] = [p.toSpec() for p in params]

    with open(spec_filepath, 'w+') as f:
        f.write(yaml.dump(specs))
    return specs

# Creates an index file for the given specs
def createIndex(specs, outdir):
    # Create index
    index = {}
    for name,spec in specs.items():
        category = ""
        if 'category' in spec:
            category = spec['category']
        if category not in index:
            index[category] = []
        spec['name'] = name
        index[category].append(spec)

    # Write File
    filepath = Path(outdir, "index.md")
    filepath.touch(exist_ok=True)
    with open(filepath, 'w+') as f:
        f.write("""---
title: Index
permalink: /functions/
---
""")
        for category, functions in index.items():
            if category == "":
                category = "Uncategorized"
            f.write("# {}\n\n".format(category))

            for spec in functions:
                params = []
                for param in spec['input-parameters']:
                    params.append(Parameter.fromSpec(param))

                param_str = ", ".join([p.toAttributeStr() for p in params])
                param_str = param_str.replace("<","\<")

                f.write("* [{}({})]({}) - {}\n".format(spec['name'],param_str, spec['name'].lower(), spec['short-description']))
            f.write("\n")


# Creates a documentation file for every function
def createDocs(specs, outdir, joda_path, ghpages: bool):
    # Create dir if it does not exist
    docs_path = Path(outdir)
    if not docs_path.exists():
        docs_path.mkdir()

    for func in specs:
        createDocFromSpec(func, specs[func], outdir, joda_path, ghpages)
    
    if ghpages:
        createIndex(specs, outdir)


def main():
    parser = argparse.ArgumentParser(
        description='Generates for each supported function in JODA a documentation page with examples.')
    parser.add_argument('--joda', dest='joda', default="joda",
                        help='Path to JODA executable (default: joda)')
    parser.add_argument('--spec', dest='spec', default="functions.yaml",
                        help='The path to the function specification yaml file (default: functions.yaml)')
    parser.add_argument('--out-dir', dest='outdir', default="functions",
                        help='A directory where the documentation files will be store (default: functions)')
    parser.add_argument('--github-pages', action='store_true', dest='ghpages')
    parser.add_argument('--evaluate-unknown-parameters',
                        action='store_true', dest='evaluateParams')
    args = parser.parse_args()

    # Fetches the specification of all available functions
    spec = createOrUpdateSpec(args.spec, args.joda)
    # Execute examples and infer types
    if args.evaluateParams:
        spec = evaluateParameters(spec, args.joda, args.spec)

    createDocs(spec, args.outdir, args.joda, args.ghpages)


if __name__ == '__main__':
    main()
