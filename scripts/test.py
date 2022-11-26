
with open("data.jpg", "wb") as myfile, open("test.jpg", "rb") as file2:
    myfile.write(file2.read())
