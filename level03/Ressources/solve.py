to_decrypt = "Q}|u`sfg~sf{}|a3"

for i in range(1, 21):
    decrypt = ""
    for j in to_decrypt:
        decrypt += chr(ord(j) ^ i)
    print(f"[{i}]: {decrypt}")

