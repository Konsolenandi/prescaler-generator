import sys
import math

def printHelp():
    print("Help will be added later")

def main():

    for index, argument in enumerate(sys.argv):
        if argument == "--clock":
            f_clk = float(sys.argv[index+1])
        if argument == "--interval":
            t_sys = float(sys.argv[index+1])

    if ('f_clk' not in locals()) or ('t_sys' not in locals()):
        printHelp()
        sys.exit()

    c = f_clk * t_sys

    n_div = [1,8,32,64,128,256,1024]
    n_ref = ["001", "010", "011", "100", "101", "110", "111"]

    g_div = -1
    g_err = math.inf
    g_cmp = -1

    for div in n_div:
        n_comp = c / div
        if n_comp > 255:
            continue
        error = abs(round(n_comp)-n_comp)

        if error < g_err:
            g_err = error
            g_div = div
            g_cmp = n_comp

        file = open("prescaler.inc", "w")
    file.write(".equ TIM2DIV = 0b" + n_ref[n_div.index(g_div)])
    file.close()

    print("div:", g_div)
    print("err:", g_err)
    print("cmp:", g_cmp)

if __name__ == "__main__":
    main()
