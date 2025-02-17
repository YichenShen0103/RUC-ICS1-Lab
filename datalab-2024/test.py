from pycparser import c_ast, parse_file
import subprocess
import re
import argparse


class FunctionAnalyzer(c_ast.NodeVisitor):
    def __init__(self):
        self.operators = []
        self.control_structures = []
        self.has_array = False
        self.function_name = None
        self.type_conversions = []
        self.ternary_ops = []

    def visit_FuncDef(self, node):
        self.function_name = node.decl.name
        self.generic_visit(node)

    def visit_BinaryOp(self, node):
        self.operators.append(node.op)
        self.generic_visit(node)
    def visit_TernaryOp(self, node):
        
        self.ternary_ops.append(node)
        
        self.generic_visit(node)

    def visit_UnaryOp(self, node):
        self.operators.append(node.op)
        self.generic_visit(node)

    def visit_Assignment(self, node):
        self.operators.append(node.op)
        self.generic_visit(node)

    def visit_If(self, node):
        self.control_structures.append("if")
        self.generic_visit(node)

    def visit_While(self, node):
        self.control_structures.append("while")
        self.generic_visit(node)

    def visit_For(self, node):
        self.control_structures.append("for")
        self.generic_visit(node)

    def visit_Switch(self, node):
        self.control_structures.append("switch")
        self.generic_visit(node)

    def visit_Decl(self, node):
        if isinstance(node.type, c_ast.ArrayDecl):
            self.has_array = True
        self.check_type(node.type)
        self.generic_visit(node)

    def visit_Cast(self, node):
        # 收集类型转换
        self.type_conversions.append((node.to_type, node.expr))
        self.generic_visit(node)

    def check_type(self, node):
        if isinstance(node, c_ast.TypeDecl):
            type_name = " ".join(node.type.names)
            self.control_structures.append(type_name)
        elif isinstance(node, (c_ast.PtrDecl, c_ast.ArrayDecl)):
            self.check_type(node.type)
        elif isinstance(node, c_ast.FuncDecl):
            self.check_type(node.type)


class FunctionFinder(c_ast.NodeVisitor):
    def __init__(self, target_func_name):
        self.target_func_name = target_func_name
        self.analyzer = None

    def visit_FuncDef(self, node):
        if node.decl.name == self.target_func_name:
            self.analyzer = FunctionAnalyzer()
            self.analyzer.visit(node)
            raise StopIteration


problem_infos = {
    "bitXor": {"rating": 1, "score": 0, "operation": ["~", "&", "int"], "maxop": 7},
    "samesign": {
        "rating": 2,
        "score": 0,
        "operation": [">>", "<<", "!", "^", "&&", "if", "else", "&", "int"],
        "maxop": 12,
    },
    "logtwo": {
        "rating": 4,
        "score": 0,
        "operation": [">", "<", ">>", "<<", "|", "int"],
        "maxop": 25,
    },
    "byteSwap": {
        "rating": 4,
        "score": 0,
        "operation": ["!", "~", "&", "^", "|", "+", "<<", ">>", "int"],
        "maxop": 17,
    },
    "reverse": {
        "rating": 3,
        "score": 0,
        "operation": [
            "<<",
            "|",
            "&",
            "-",
            "+",
            ">>",
            "for",
            "while",
            "!",
            "~",
            "unsigned",
            "int",
            "unsigned int",
        ],
        "maxop": 30,
    },
    "logicalShift": {
        "rating": 3,
        "score": 0,
        "operation": ["!", "~", "&", "^", "|", "+", "<<", ">>", "int"],
        "maxop": 20,
    },
    "leftBitCount": {
        "rating": 4,
        "score": 0,
        "operation": ["!", "~", "&", "^", "|", "+", ">>", "<<", "int"],
        "maxop": 50,
    },
    "float_i2f": {
        "rating": 4,
        "score": 0,
        "operation": [
            "if",
            "else",
            "while",
            "for",
            "&",
            "|",
            "~",
            "+",
            "-",
            "<<",
            ">>",
            "<",
            ">",
            "!",
            "==",
            "unsigned",
            "int",
            "unsigned int",
        ],
        "maxop": 30,
    },
    "floatScale2": {
        "rating": 4,
        "score": 0,
        "operation": [
            "&",
            "<<",
            ">>",
            "|",
            "if",
            ">",
            "<",
            ">=",
            "<=",
            "!",
            "~",
            "else",
            "+",
            "==",
            "int",
            "unsigned",
            "unsigned int",
        ],
        "maxop": 30,
    },
    "float64_f2i": {
        "rating": 3,
        "score": 0,
        "operation": [
            ">>",
            "<<",
            "|",
            "&",
            "~",
            "!",
            "+",
            "-",
            ">",
            "<",
            ">=",
            "<=",
            "if",
            "else",
            "int",
            "unsigned",
            "unsigned int",
        ],
        "maxop": 60,
    },
    "floatPower2": {
        "rating": 4,
        "score": 0,
        "operation": [
            "<",
            ">",
            "<=",
            ">=",
            "<<",
            ">>",
            "+",
            "-",
            "&",
            "|",
            "~",
            "!",
            "if",
            "else",
            "&&",
            "int",
            "unsigned",
            "unsigned int",
        ],
        "maxop": 30,
    },
}


def test_legality(ast, function):
    finder = FunctionFinder(function)
    try:
        finder.visit(ast)
    except StopIteration:
        pass
    problem_info = problem_infos[function]
    analyzer = finder.analyzer
    function_name = analyzer.function_name
    operators_temp = [i for i in analyzer.operators if i != "="]
    operators = []
    for i in operators_temp:
        if "--" in i:
            operators.append("-")
        elif "++" in i:
            operators.append("+")
        else:
            operators.append(i)
    illegal = set()
    if analyzer.has_array:
        illegal.add("array")
    if len(analyzer.ternary_ops) !=0:
        operators.append("? :")
    for i in operators:
        if i != "==":
            i = i.replace("=", "")
        if i not in problem_info["operation"]:
            illegal.add(i)
    for i in analyzer.control_structures:
        if i not in problem_info["operation"]:
            illegal.add(i)

    legality_message = []

    if len(illegal) != 0:
        legality_message.append(f"Using illegal operations: {illegal}.")

    if len(operators) > problem_info["maxop"]:
        legality_message.append(
            f"Using excessive operations, you use {len(operators)} > max ops {problem_info['maxop']}."
        )

    if len(analyzer.type_conversions) != 0:
        legality_message.append(f"Using type conversion.")

    pass_message = [
        f"Pass using {len(operators)} operations including {set(operators)} and {set(analyzer.control_structures)}."
    ]

    return len(legality_message) == 0, legality_message, pass_message


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--legality_only", action="store_true")
    parser.add_argument("--verbose", "-V", action="store_true")
    args = parser.parse_args()

    try:
        subprocess.run(["make", "clean"], check=True)
        subprocess.run(["make"], check=True)
        print("Make success.")
    except subprocess.CalledProcessError as e:
        print(f"Error while running make: {e}.")
        exit(1)

    ast = parse_file("bits.c", use_cpp=True)

    results = {
        function_name: {
            "correctness": False,
            "correctness_message": [],
            "legality": False,
            "legality_message": [],
            "legality_pass_message": [],
        }
        for function_name in problem_infos.keys()
    }

    if args.legality_only:
        for function_name in problem_infos.keys():
            results[function_name]["correctness"] = False
            results[function_name]["correctness_message"] = [
                "Correctness unknown, legality only."
            ]
    else:
        with open("result.txt", "w") as f:
            try:
                subprocess.run(["./btest"], stdout=f, check=True)
            except subprocess.CalledProcessError as e:
                print(f"Error while running btest: {e}.")
                exit(1)

        with open("result.txt", "r") as rd:
            lines = rd.readlines()

        for line1, line2 in zip(lines, lines[1:] + [""]):
            body = line1.strip() + line2.strip()
            if "Score" in line1:  # first line
                continue
            if "Total points" in line1:  # last line
                continue
            elif "ERROR" in line1:
                matches = re.findall(r"\b\w+\b|\b\d+\b", line1)
                function_name = matches[2]
                results[function_name]["correctness"] = False
                body = body.replace("ERROR: ", "")
                body = body.replace("......", ". ")
                results[function_name]["correctness_message"] = [body]
            elif "Gives" in line1:
                continue
            else:
                matches = re.findall(r"\b\w+\b|\b\d+\b", line1)
                function_name = matches[-1]
                results[function_name]["correctness"] = True

        for function_name in problem_infos.keys():
            legality, legality_message, legality_pass_message = test_legality(
                ast, function_name
            )
            results[function_name]["legality"] = legality
            results[function_name]["legality_message"] = legality_message
            results[function_name]["legality_pass_message"] = legality_pass_message

    total_points = 0
    for function_name, result in results.items():
        is_pass = result["correctness"] and result["legality"]
        earn_points = problem_infos[function_name]["rating"] if is_pass else 0
        msg_header = f"{function_name:<15} {earn_points}/{problem_infos[function_name]['rating']}:"
        msg_mid = "PASS" if is_pass else "FAIL"
        msg_bodies = []
        if not is_pass:
            msg_bodies = result["correctness_message"] + result["legality_message"]
        elif args.verbose:
            msg_bodies = result["legality_pass_message"]

        total_points += earn_points

        if len(msg_bodies) == 0:
            print(f"{msg_header:<25}{msg_mid}")
        else:
            print(f"{msg_header:<25}{msg_mid:<8}", end="")
            for i, msg_body in enumerate(msg_bodies):
                if i == 0:
                    if not is_pass:
                        print(f"error{i+1}: {msg_body}")
                    else:
                        print(f"detail: {msg_body}")
                else:
                    print(f"{' ' *33}error{i+1}: {msg_body}")

    max_points = sum(problem_info["rating"] for problem_info in problem_infos.values())
    print(f"Total points: {total_points}/{max_points}")

    # AutoGrader
    with open(".autograder_result", "w") as f:
        f.write(f"{total_points}")


if __name__ == "__main__":
    main()
