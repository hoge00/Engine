import platform
import subprocess
import shutil

import os
import sys

def print_on_console(line):
    print(line)
    sys.stdout.flush()


class OreExample(object):
    def __init__(self, dry=False):
        self.ore_exe = ""
        self.headlinecounter = 0
        self.dry = dry
        self.ax = None
        self.plot_name = ""
        self._locate_ore_exe()

    def _locate_ore_exe(self):
        if os.name == 'nt':
            if platform.machine()[-2:] == "64":
                if os.path.isfile("..\\..\\App\\bin\\x64\\Release\\ore.exe"):
                    self.ore_exe = "..\\..\\App\\bin\\x64\\Release\\ore.exe"
                elif os.path.isfile("..\\..\\build\\App\\ore.exe"):
                    self.ore_exe = "..\\..\\build\\App\\ore.exe"
                else:
                    print_on_console("ORE executable not found.")
                    quit()
            else:
                if os.path.isfile("..\\..\\App\\bin\\Win32\\Release\\ore.exe"):
                    self.ore_exe = "..\\..\\App\\bin\\Win32\\Release\\ore.exe"
                elif os.path.isfile("..\\..\\build\\App\\ore.exe"):
                    self.ore_exe = "..\\..\\build\\App\\ore.exe"
                else:
                    print_on_console("ORE executable not found.")
                    quit()
        else:
            if os.path.isfile("../../App/build/ore"):
                self.ore_exe = "../../App/build/ore"
            elif os.path.isfile("../../build/App/ore"):
                self.ore_exe = "../../build/App/ore"
            elif os.path.isfile("../../App/ore"):
                self.ore_exe = "../../App/ore"
            else:
                print_on_console("ORE executable not found.")
                quit()

    def print_headline(self, headline):
        self.headlinecounter += 1
        print_on_console('')
        print_on_console(str(self.headlinecounter) + ") " + headline)

    def get_times(self, output):
        print_on_console("Get times from the log file:")
        logfile = open(output)
        for line in logfile.readlines():
            if "ValuationEngine completed" in line:
                times = line.split(":")[-1].strip().split(",")
                for time in times:
                    print_on_console("\t" + time.split()[0] + ": " + time.split()[1])

    def get_output_data_from_column(self, csv_name, colidx, offset=1):
        f = open(os.path.join(os.path.join(os.getcwd(), "Output"), csv_name))
        data = []
        for line in f:
            if colidx < len(line.split(',')):
                data.append(line.split(',')[colidx])
            else:
                data.append("Error")
        return [float(i) for i in data[offset:]]

    def save_output_to_subdir(self, subdir, files):
        if not os.path.exists(os.path.join("Output", subdir)):
            os.makedirs(os.path.join("Output", subdir))
        for file in files:
            shutil.copy(os.path.join("Output", file), os.path.join("Output", subdir))

    def run(self, xml):
        if not self.dry:
            if subprocess.call([self.ore_exe, xml]) != 0:
                raise Exception("Return Code was not Null.")
