import sys
import argparse
from XY_433 import XY_433_trans

def getArgs():
    parser = argparse.ArgumentParser(description="send code vai XY_433 transmitter")
    parser.add_argument('-c', '--code', action="store", dest='code', type=int,  required=True, help="code to be sent")
    parser.add_argument('-b', '--bits', action="store", dest='bits', type=int,  required=False, default=24, help="number of bit of code")
    parser.add_argument('-p', '--pin', action="store", dest='pin', type=int,  required=False, default=2, help="pin XY_433 transmitter is on")
    parser.add_argument('-r', '--repeat', action="store", dest='repeat', type=int,  required=False, default=3, help="number of times code is sent")
    return parser.parse_args()
    


def main():
    args = getArgs()
    trans = XY_433_trans(args.pin)
    trans.XY_send(args.code, args.bits, args.repeat)
    trans.close()
    exit()



if __name__ == "__main__":
    main()