#!/usr/bin/env python3
import json
import sys


def parse_nm_output(output):
    """Parse nm output into a set of (type, name) tuples"""
    symbols = set()
    if not output:
        return symbols

    for line in output.strip().split("\n"):
        if not line:
            continue
        # Each line format: "0000000000000000 T main"
        try:
            addr, type_, name = line.split()
            # Skip section symbols (like .text, .data, etc)
            if name.startswith("."):
                continue
            symbols.add((type_.strip(), name.strip()))
        except ValueError:
            continue
    return symbols


def format_symbol(type_, name):
    """Format a symbol for display in error messages"""
    return f"{name}({type_})"


def judge():
    # Read test data from stdin
    input_data = json.load(sys.stdin)

    # Handle stdout which might be bytes or str
    stdout = input_data.get("stdout", "")
    if isinstance(stdout, bytes):
        stdout = stdout.decode("utf-8", errors="ignore")
    stdout = stdout.strip()

    # Read expected output
    with open(input_data["test_dir"] + "/ans.out", "r", encoding="utf-8") as f:
        expected = f.read().strip()

    # Parse both outputs
    actual_symbols = parse_nm_output(stdout)
    expected_symbols = parse_nm_output(expected)

    # Compare sets (order independent)
    if actual_symbols == expected_symbols:
        result = {"success": True, "message": "Output matches expected symbols"}
    else:
        # Create dictionaries for easier comparison
        actual_dict = {name: type_ for type_, name in actual_symbols}
        expected_dict = {name: type_ for type_, name in expected_symbols}

        # Compare symbol by symbol
        message = []

        # Check for missing and wrong type symbols
        for name, exp_type in expected_dict.items():
            if name not in actual_dict:
                message.append(f"Missing symbol: {format_symbol(exp_type, name)}")
            else:
                act_type = actual_dict[name]
                if act_type != exp_type:
                    message.append(
                        f"Mismatch for {name}: expected {format_symbol(exp_type, name)}, "
                        f"got {format_symbol(act_type, name)}"
                    )

        # Check for extra symbols
        for name in actual_dict:
            if name not in expected_dict:
                type_ = actual_dict[name]
                message.append(f"Extra symbol: {format_symbol(type_, name)}")

        if not message:  # Should never happen, but just in case
            message = [
                "Outputs differ but no specific differences found. This is a bug in the judge."
            ]

        result = {"success": False, "message": " | ".join(message)}

    # Ensure output is properly encoded
    print(json.dumps(result, ensure_ascii=False))


if __name__ == "__main__":
    judge()
