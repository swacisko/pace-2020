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

# inst_dir = "normalized_with_clouds_single_triangle"
inst_dir = 'input'
output_root_dir = 'results'
solver_name = 'extreem'

# results will be printed to a separate file

# this program will run [thread_cnt] processes, each running TestsRunner, which runs tests_runner_threads processes,
# each of which calls the solver process...
thread_cnt = 2
tests_runner_threads = 4

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
          ' --skip_existing_results=true'

    print('\nCreating tables and ranking, running command', cmd)
    os.system(cmd)


all_tests_commands = []


# separator_creators = ['ArtPointCr', 'BfsCr', 'CompExpCr', 'FlowCr', 'FlowCutterCr']
separator_creators = ['ArtPointCr', 'BfsCr', 'CompExpCr', 'FlowCutterCr']
separator_minmizers = [ 'BfsMinim', 'ExpansionMinim', 'FlowCutterMinim', 'FlowCutterDstMinim', 'FlowMinim', 'GNEMinim', 'NeighVCCMinim']
preprocessing_types = [ 'ArtPointsPrepr', 'IndSet3Prepr', 'IndSet4Prepr', 'DanglingTrees',]
pivot_types = ['BlockPivots', 'HallSetPivots']
def_time = 2 * 3600 # default time of 5h
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


def createMainRepsCommands():
    for pred_conf in [4, 3, 2, 1]:
        for main_reps in np.arange(30,4,-5):
            cmd = getDefaultCommand()
            cmd += ' --run_name=main_reps_and_pred_conf__' + str(main_reps) + '_' + str(pred_conf)
            solver_params = '--experiment_name=main_reps_and_pred_conf' + \
                            ' --time=' + str(5 * def_time) + \
                            ' --pred_conf=' + str(pred_conf)
            cmd += ' --solver_params=\'' + solver_params + '\''
            all_tests_commands.append(cmd)


def createPreprocessingCommands():
    def getCmd(x,y):
        cmd = getDefaultCommand()
        cmd += ' --run_name=prepr__' + parsePreprocessing(x)
        solver_params = '--experiment_name=prepr' + \
                        ' --time=' + str(def_time) + \
                        ' --pred_conf=1' + \
                        ' --init_prepr=true' + \
                        ' --prepr_mask=' + str(y)
        cmd += ' --solver_params=\'' + solver_params + '\''
        return cmd

    for prepr in range(len(preprocessing_types)):
        all_tests_commands.append(getCmd(prepr, 1<<prepr))

    all_tests_commands.append(getCmd(-1,0)) # no prepr
    all_tests_commands.append( getCmd( len(preprocessing_types), (1 << len(preprocessing_types))-1 ) ) # all prepr


def createInitPreprocessingAndPredefinedConfigsCommands():
    for pred_conf in [4,3,2,1]:
        for init_prepr in [0,1]:
            cmd = getDefaultCommand()
            cmd += ' --run_name=init_prepr_and_pred_conf__' + str(init_prepr) + '_' + str(pred_conf)
            solver_params = '--experiment_name=init_prepr_and_pred_conf' + \
                            ' --time=' + str(5*def_time) + \
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
                        ' --init_prepr=true' + \
                        ' --pred_conf=1'
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
                        ' --pred_conf=1' + \
                        ' --pivots_mask=0' + \
                        ' --prepr_mask=0' + \
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
    createPreprocessingCommands()
    createInitPreprocessingAndPredefinedConfigsCommands()
    createMainRepsCommands()



def runTestForCommand(cmd):
    print('Running command', cmd)
    os.system(cmd)

print(f'{(platform.system())=}')

createTestsCommands()

print('#CAUTION! Taking only a fraction of all tests, just to test if it works as intended...')
all_tests_commands = all_tests_commands[0:5]
print("All commands to run:", *all_tests_commands, sep='\n\n', end='\n\n')

p = multiprocessing.Pool(thread_cnt)
dss = p.map(runTestForCommand, all_tests_commands, chunksize=1)
createTablesAndRankings()