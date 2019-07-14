import sys
import math

def printHelp():
    print("Help will be added later")

def readInputFile(inputFile):
    fclk = -1
    settings = open(inputFile, "r")
    for line in settings:
        if line.startswith(".equ FCLK"):
            fclk = [int(s) for s in line.split() if s.isdigit()]
    
    settings.close()
    
    if fclk[0] < 0:
        print("Couldn't get information from given file")
        sys.exit()
    return fclk[0]

def main():

    for index, argument in enumerate(sys.argv):
        if argument == "--clock":
            f_clk = float(sys.argv[index+1])
        if argument == "--interval":
            t_sys = float(sys.argv[index+1])
#        if argument == "--timer":
#            # ToDo
        if argument == "--file" or argument == "-f":
            inputFile = sys.argv[index+1]
        
    if ('inputFile' in locals()):
            f_clk = 1000000*readInputFile(inputFile)

    if ('f_clk' not in locals()) or ('t_sys' not in locals()):
        printHelp()
        sys.exit()

    c = f_clk * t_sys

    timer2 = True
    timer0 = False
    timer1 = False

    if timer2:
        n_div = [1,8,32,64,128,256,1024]
        n_ref = ["001", "010", "011", "100", "101", "110", "111"]
    elif timer0 or timer1:
        n_div = [1,8,64,256,1024]
        n_ref = ["001", "010", "011", "100", "101"] 
    else:
        print("No timer specified")
        sys.exit()

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
    file.write(".equ TIM2DIV = 0b" + n_ref[n_div.index(g_div)] + "\n")
    file.write(".equ TIM2COMPVAL = " + str(int(g_cmp)))
    file.close()

    print("div:", g_div)
    print("err:", g_err)
    print("cmp:", g_cmp)

if __name__ == "__main__":
    main()
