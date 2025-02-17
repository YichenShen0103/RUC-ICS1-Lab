import argparse
import os


def main(matrix_name, rows, cols):
    os.system("./csim-show -s 4 -E 1 -b 5 -t ./gemm_traces/case3.trace > ./miss.txt")

    array = [[0 for _ in range(37)] for _ in range(37)]
    count = 0
    with open("miss.txt", 'r') as f:
        for line in f:
            lst = line.split(" ")
            if lst[1] == matrix_name:
                array[int(lst[2])][int(lst[3])] += 1
                count += 1

    for i in range(rows):
        for j in range(cols):
            if array[i][j] != 0:
                print(array[i][j], end=" ")
            else:
                print("-", end=" ")
        print()

    print(count)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="统计并可视化 miss.txt 中的 miss 次数")
    parser.add_argument("-n", "--name", type=str, help="矩阵名", default="A")
    parser.add_argument("-r", "--row", type=int, help="矩阵的行数", default=37)
    parser.add_argument("-c", "--col", type=int, help="矩阵的列数", default=37)
    args = parser.parse_args()
    main(args.name, args.row, args.col)
