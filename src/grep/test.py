#!/usr/bin/python

from os import system


flags = 'ivclnhso'
files = ['test1.txt', 'test2.txt']
patterns = ['char', 'a', 'error']


def file_compare(file1, file2):
    err = 1
    with open(file1, "r") as file_1:
        with open(file2, "r") as file_2:
            a, b = file_1.read(), file_2.read()
            if a != b:
                err = 0

    return err


def test(flag, test_file, pattern):
    system("grep -{fl} {pat} {file} > grep_out.txt".format(fl=flag,
           pat=pattern, file=test_file))
    system("./my_grep -{fl} {pat} {file} > out.txt".format(fl=flag,
           pat=pattern, file=test_file))

    if file_compare("grep_out.txt", "out.txt"):
        return 1
    return 0


def test_f(flag, test_file):
    system(
        "grep -{fl} some {file} > grep_out.txt".format(fl=flag,  file=test_file))
    system(
        "./my_grep -{fl} some {file} > out.txt".format(fl=flag, file=test_file))

    if file_compare("grep_out.txt", "out.txt"):
        return 1
    return 0


def test_2f(flag1, flag2, pattern, test_file):
    system("grep -{fl1}{fl2} {pat} {file} > grep_out.txt".format(fl1=flag1,
           fl2=flag2, pat=pattern, file=test_file))
    system("./my_grep -{fl1}{fl2} {pat} {file} > out.txt".format(
        fl1=flag1, fl2=flag2, pat=pattern, file=test_file))

    if file_compare("grep_out.txt", "out.txt"):
        return 1
    return 0


success = 0
fail = 0

for i in flags:
    for j in files:
        for m in patterns:
            res = test(i, j, m)
            if res:
                success += 1
                print("OK")
            else:
                fail += 1
                print("FAIL")

for i in flags:
    for j in flags:
        for m in patterns:
            for f in files:
                res = test_2f(i, j, m, f)
                if res:
                    success += 1
                    print("OK")
                else:
                    fail += 1
                    print("FAIL")


for j in files:
    res = test_f('f', j)
    if res:
        success += 1
        print("OK")
    else:
        fail += 1
        print("FAIL")

print("------------")

print("SUCCESS: {succ}".format(succ=success))
print("FAIL: {f}".format(f=fail))
