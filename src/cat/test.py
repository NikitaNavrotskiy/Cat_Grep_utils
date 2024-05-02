#!/usr/bin/python

from os import system


flags = 'bensvt'
files = ['test1.txt', 'test2.txt']

failes = 0
success = 0


def file_compare(file1, file2):
    err = 1
    with open(file1, "r") as file_1:
        with open(file2, "r") as file_2:
            a, b = file_1.read(), file_2.read()
            if a != b:
                err = 0

    return err


def test(flag, test_file):
    system("cat -{fl} {file} > cat_out.txt".format(fl=flag, file=test_file))
    system("./s21_cat -{fl} {file} > out.txt".format(fl=flag, file=test_file))

    if file_compare("cat_out.txt", "out.txt"):
        return 1
    return 0


for i in flags:
    for j in files:
        res = test(i, j)
        if res:
            success += 1
            print("OK")
        else:
            failes += 1
            print("FAIL", i, j)

print("-----------------")

print("SUCCESS: {suc}".format(suc=success))
print("FAILED: {fail}".format(fail=failes))
