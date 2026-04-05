import csv
import json
import os
import sys
import shutil
import concurrent.futures
import multiprocessing
import timeit
import time
import platform
import argparse
import numpy as np
import itertools
from pathlib import Path

RUN_TESTS = False

inst_dir = 'input_small_tests' if RUN_TESTS else 'extreem-instances'
output_root_dir = 'results_small_tests' if RUN_TESTS else 'results'

# inst_dir = 'input_small_for_tests'
# output_root_dir = 'results_small_tests'
# inst_dir = "extreem-instances"
# output_root_dir = 'results'
solver_name = 'extreem'


# this program will run [thread_cnt] processes, each running TestsRunner, which runs tests_runner_threads processes,
# each of which calls the solver process...
thread_cnt = 2 if not RUN_TESTS else 4
tests_runner_threads = 4 if not RUN_TESTS else 5

# thread_cnt = 1
# tests_runner_threads = 1

def getDefaultCommand():
    cmd = 'python3 TestsRunner.py' + \
          ' --instances_dir=' + inst_dir + \
          ' --output_root_dir=' + output_root_dir + \
          ' --threads=' + str(tests_runner_threads) + \
          ' --solver_name=' + solver_name + \
          ' --remove_existing_results=false' + \
          ' --skip_existing_results=true' + \
          ' --compute=true' + \
          ' --run_judge=false' + \
          ' --create_rankings=false'
    return cmd

def createTablesAndRankings():
    cmd = 'python3 TestsRunner.py' + \
          ' --instances_dir=' + inst_dir + \
          ' --output_root_dir=' + output_root_dir + \
          ' --compute=true' + \
          ' --run_solver=false' + \
          ' --run_judge=false' + \
          ' --create_verdict_file=false' + \
          ' --create_rankings=true' + \
          ' --threads=1' + \
          ' --solver_name=' + solver_name + \
          ' --remove_existing_results=false' + \
          ' --skip_existing_results=true' + \
          ' --report_runs_in_separate_lines=true'

    print('\nCreating tables and ranking, running command', cmd)
    os.system(cmd)


all_tests_commands = []


# separator_creators = ['ArtPointCr', 'BfsCr', 'CompExpCr', 'FlowCr', 'FlowCutterCr']
separator_creators = ['ArtPointCr', 'BfsCr', 'CompExpCr', 'FlowCutterCr']
separator_minmizers = [ 'BfsMinim', 'ExpansionMinim', 'FlowCutterMinim', 'FlowCutterDstMinim', 'FlowMinim', 'GNEMinim', 'NeighVCCMinim']
preprocessing_types = [ 'ArtPointsPrepr', 'DanglingTrees', 'IndSet3Prepr', 'IndSet4Prepr']
pivot_types = ['BlockPivots', 'HallSetPivots']
def_time = 5 * 3600 # default time of 5h
def parseSepCr(x):
    if x == -1: return 'no'
    if x == len(separator_creators):
        return 'all'
    return separator_creators[x]

def parseSepMinim(x):
    if x == -1: return 'no'
    if x == len(separator_minmizers):
        return 'all'
    return separator_minmizers[x]

def parsePivots(x):
    if x == -1: return 'no'
    if x == len(pivot_types):
        return 'all'
    return pivot_types[x]

def parsePreprocessing(x):
    if x == -1: return 'no'
    if x == len(preprocessing_types):
        return 'all'
    return preprocessing_types[x]


def createNSFCommands():
    for nsf in np.linspace(0.0,1.0,10):
        cmd = getDefaultCommand()
        cmd += ' --run_name=nsf__'  + str(f"{nsf:.3f}")
        solver_params = '--experiment_name=nsf' + \
                        ' --time=' + str(def_time) + \
                        ' --nsf=' + str(f"{nsf:.3f}") + \
                        ' --pred_conf=3' + \
                        ' --main_reps=5'
        cmd += ' --solver_params=\'' + solver_params + '\''
        all_tests_commands.append(cmd)

def createPredConfCommands():
    for pred_conf in [4, 3, 2, 1]:
        cmd = getDefaultCommand()
        cmd += ' --run_name=pred_conf__'  + str(pred_conf)
        solver_params = '--experiment_name=pred_conf' + \
                        ' --time=' + str(3 * def_time) + \
                        ' --pred_conf=' + str(pred_conf)
        cmd += ' --solver_params=\'' + solver_params + '\''
        all_tests_commands.append(cmd)

def createMainRepsCommands():
    # for pred_conf in [3, 2, 1]:
    for pred_conf in [2]:
        for main_reps in np.arange(30,4,-5):
            cmd = getDefaultCommand()
            cmd += ' --run_name=main_reps_and_pred_conf__' + str(main_reps) + '_' + str(pred_conf)
            solver_params = '--experiment_name=main_reps_and_pred_conf' + \
                            ' --time=' + str(def_time) + \
                            ' --pred_conf=' + str(pred_conf) + \
                            ' --main_reps=' + str(main_reps)
            cmd += ' --solver_params=\'' + solver_params + '\''
            all_tests_commands.append(cmd)


def createPreprocessingCommands():
    def getCmd(x,y):
        cmd = getDefaultCommand()
        cmd += ' --run_name=prepr__' + parsePreprocessing(x)
        solver_params = '--experiment_name=prepr' + \
                        ' --time=' + str(def_time) + \
                        ' --prepr_mask=' + str(y) + \
                        ' --pred_conf=1' + \
                        ' --init_prepr=true'
        cmd += ' --solver_params=\'' + solver_params + '\''
        return cmd

    for prepr in range(len(preprocessing_types)):
        all_tests_commands.append(getCmd(prepr, 1<<prepr))

    all_tests_commands.append(getCmd(-1,0)) # no prepr
    all_tests_commands.append( getCmd( len(preprocessing_types), (1 << len(preprocessing_types))-1 ) ) # all prepr


def createInitPreprocessingAndPredefinedConfigsCommands():
    for pred_conf in [3,2,1]:
        for init_prepr in [0,1]:
            cmd = getDefaultCommand()
            cmd += ' --run_name=init_prepr_and_pred_conf__' + str(init_prepr) + '_' + str(pred_conf)
            solver_params = '--experiment_name=init_prepr_and_pred_conf' + \
                            ' --time=' + str(2*def_time) + \
                            ' --pred_conf=' + str(pred_conf) + \
                            ' --init_prepr=' + str(init_prepr)
            cmd += ' --solver_params=\'' + solver_params + '\''
            all_tests_commands.append(cmd)

def createPivotsCommands():
    def getCmd(x,y):
        cmd = getDefaultCommand()
        cmd += ' --run_name=pivots__' + parsePivots(x)
        solver_params = '--experiment_name=pivots' + \
                        ' --time=' + str(def_time) + \
                        ' --pivots_mask=' + str(y) + \
                        ' --pred_conf=1' + \
                        ' --init_prepr=true'
        cmd += ' --solver_params=\'' + solver_params + '\''
        return cmd

    for pivots in range(len(pivot_types)):
        all_tests_commands.append(getCmd(pivots, 1<<pivots))

    all_tests_commands.append(getCmd(-1, 0)) # no pivots
    all_tests_commands.append(getCmd(len(pivot_types), (1<<len(pivot_types))-1)) # all pivots

def createSepMinimCommands():
    def getCmd(x,y):
        cmd = getDefaultCommand()
        cmd += ' --run_name=sep_minim__' + parseSepMinim(x)
        solver_params = '--experiment_name=sep_minim' + \
                        ' --time=' + str(def_time) + \
                        ' --sep_minim_mask=' + str(y) + \
                        ' --pivots_mask=0' + \
                        ' --prepr_mask=0' + \
                        ' --pred_conf=1' + \
                        ' --init_prepr=true'
        cmd += ' --solver_params=\'' + solver_params + '\''
        return cmd

    for sep_minim in range(len(separator_minmizers)):
        all_tests_commands.append(getCmd(sep_minim, 1 << sep_minim))

    all_tests_commands.append(getCmd(-1, 0)) # no minimizers
    all_tests_commands.append(getCmd(len(separator_minmizers), (1<<len(separator_minmizers))-1)) # all minimizers

def createSepCrCommands():
    def getCmd(x,y):
        cmd = getDefaultCommand()
        cmd += ' --run_name=sep_cr__' + parseSepCr(x)
        solver_params = '--experiment_name=sep_cr' + \
                        ' --time=' + str(def_time) + \
                        ' --sep_cr_mask=' + str(y) + \
                        ' --pred_conf=0' + \
                        ' --init_prepr=true'
        cmd += ' --solver_params=\'' + solver_params + '\''
        return cmd

    for sep_cr in range(len(separator_creators)):
        all_tests_commands.append(getCmd(sep_cr, 1<<sep_cr))

    all_tests_commands.append(getCmd(len(separator_creators), (1<<len(separator_creators))-1)) # all creators

def createTestsCommands():

    createSepCrCommands()
    createSepMinimCommands()
    createPivotsCommands()
    createPredConfCommands()
    createPreprocessingCommands()
    # createInitPreprocessingAndPredefinedConfigsCommands()
    createMainRepsCommands()
    createNSFCommands()

    print(f'\nThere are altogether {len(all_tests_commands)} commands to run in total')

def runTestForCommand(cmd):
    print('Running command', cmd)
    os.system(cmd)

def count_files(root_dir, extensions):
    root = Path(root_dir)
    extensions = {ext.lower() for ext in extensions}  # normalize

    return sum(
        1 for f in root.rglob("*")
        if f.is_file() and f.suffix.lower() in extensions
    )

# Example


if __name__ == '__main__':
    all_input_files = count_files(inst_dir, ['.txt', '.in', '.mtx', '.edges'])
    print(all_input_files)

    print(f'{(platform.system())=}')
    print(f'{RUN_TESTS=} {thread_cnt=} {tests_runner_threads=} {all_input_files=}')

    try:
        if not os.path.exists(output_root_dir):
            os.makedirs(output_root_dir)
    except Exception as e:
        print(f"Error creating directory: {e}")

    createTestsCommands()

    if RUN_TESTS:
        print('#CAUTION! Taking only a fraction of all tests, just to test if it works as intended...')
        # all_tests_commands = all_tests_commands[0:5] # take first 5 elements
        all_tests_commands = all_tests_commands[0::11]
        # all_tests_commands = all_tests_commands[-1:]

    print(f"All {len(all_tests_commands)} commands to run:", *all_tests_commands, sep='\n\n', end='\n\n')

    p = multiprocessing.Pool(thread_cnt)
    dss = p.map(runTestForCommand, all_tests_commands, chunksize=1)
    createTablesAndRankings()