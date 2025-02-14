import random

def generate_large_file(filename, size_mb, min_val=0, max_val=1000):
    target_size = size_mb * 1024 * 1024
    avg_line_size = len(str(max_val)) + 1  

    with open(filename, 'w') as out_file:
        written_size = 0
        while written_size < target_size:
            num = random.randint(min_val, max_val)
            line = f"{num}\n"
            out_file.write(line)
            written_size += len(line)

    print(f"Файл {filename} создан. Размер: {written_size / (1024 * 1024):.2f} МБ")

if __name__ == "__main__":
    filename = "random_numbers256.txt"
    size_mb = 256 

    generate_large_file(filename, size_mb)