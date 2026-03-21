import shutil
from pathlib import Path
import os
import sys
from itertools import count
from concurrent.futures import ThreadPoolExecutor
import time

threads = 8


def readGraphFromFile(f, extension):
    V = []
    
    if extension in ['.mtx', '.edges']:
        #print(f'\t\tReading a .mtx graph format')
        lines = f.readlines()
        lines = list(filter(lambda l : not l.startswith('%') and not l.startswith('c'),lines))
        lines = [l.strip() for l in lines]
        
        edges = []
        
        if extension == '.mtx':
            fl = lines[0]
            lines = lines[1:] 
            print(f'{fl=}')
            if len(fl) != 3:
                fl = [int(x) for x in fl.split()]
                edges.append((fl[0], fl[1]))
                  
            
        for l in lines:
            l = l.split()
            if extension == '.edges':
                if len(l) == 1 and ',' in l[0]:
                    l = l[0].split(',')
            #print(l,l[0])
            edges.append((int(l[0]), int(l[1])))
            
        min_id = min(min(a, b) for a, b in edges)
        max_id = max(max(a, b) for a, b in edges)
        if min_id > 0:
            edges = [(a-min_id, b-min_id) for a,b in edges]
            
        N = max_id - min_id + 1
        M = len(edges)
        V = [[] for _ in range(N)]
        for a,b in edges:
            V[a].append(b)
            V[b].append(a)
        V = [ list(set(v)) for v in V ]
        V = [[d for d in v if d != i] for i, v in enumerate(V)]
            
        
    elif extension in ['.txt', '.in']:
        return []
        
    return V
    
def countEdges(V):
    return sum(map(lambda v : len(v),V))
        
def checkGraph(V):
    nodes = len(V)
    edge_cnt = countEdges(V)
    avg_deg = 2*edge_cnt / nodes
    max_deg = max(map(lambda v : len(v),V))
    
    
    max_nodes_allowed = 1e6
    max_edges_allowed = 1e6
    max_deg_allowed = 10**9
    print(f'\t\t\t{nodes=}, {edge_cnt=}, {avg_deg=}, {max_deg=}  <--->  {max_nodes_allowed=}, {max_edges_allowed=}, {max_deg_allowed=}')
    
    return nodes <= max_nodes_allowed and edge_cnt <= max_edges_allowed and max_deg <= max_deg_allowed


def readAndFilterGraph(file_path : Path) -> bool:
    accept = False
    V = []
    print('\t\tReading graph from file', file_path)
    with open(file_path,'r',encoding='utf-8-sig') as f:
        V = readGraphFromFile(f,file_path.suffix)
        accept = checkGraph(V)
        #print(f'\t\t{len(V)=}')
        
    return accept,V
    
    

def graphFilter(file_path: Path) -> bool:
    """
    Replace this stub with your real filtering logic.
    Return True if the file should be copied, otherwise False.
    """
    # Example:
    # return file_path.stat().st_size - gets size of the file in bytes
    
    #return 'massive' not in str(file_path)
    #return file_path.stat().st_size < 100e6 #100 MB max size
    
    if 'massive' in str(file_path): return False, []
    return readAndFilterGraph(file_path)


def writeGraphToFile(V, file_path, graph_format):
    if graph_format == 'std1':
        with open(file_path,'w') as f:
            edge_cnt = countEdges(V)
            print(len(V), edge_cnt, sep=' ', file=f)
            for i,v in enumerate(V):
                print(*[ f'{i} {d}' for d in v], sep='\n', file=f)
        print(f'Wrote graph with {edge_cnt=}')        
    else:
        assert(False and 'only std graph format supported now')
      
      
graphs_checked = count()
graphs_accepted = count()

def worker(args):
    file_path, root_path, output_path = args
    global graphs_checked, graphs_accepted
    
    try:
        next(graphs_checked)
        accept,V = graphFilter(file_path)
        if accept:
            next(graphs_accepted)
            relative_path = file_path.relative_to(root_path)
            destination = output_path / relative_path

            destination.parent.mkdir(parents=True, exist_ok=True)
            
            #shutil.copy2(file_path, destination)
            #print(f"Copied: {file_path} -> {destination}")
            
            destination = destination.with_suffix('.txt')
            graph_format = 'std1'
            writeGraphToFile(V,destination,graph_format)
            print(f'Wrote graph from {file_path} to {destination} in {graph_format} format\n\n')
        else:
            print(f"\t Skipped by filter: {file_path}")

    except Exception as e:
        print(f"\n\nError processing {file_path}: {e}   !!!!!!!   !!!!! !!!!!\n\n")
        exit(2)
      

def filter_and_copy_files(root_dir, output_dir, allowed_extensions):
    """
    Iterate over all files in root_dir (including subdirectories).
    For each file with an allowed extension, call graphFilter().
    If graphFilter() returns True, copy the file into output_dir
    while preserving the relative subdirectory structure.

    :param root_dir: Root directory to search
    :param output_dir: Destination root directory
    :param allowed_extensions: Iterable of extensions, e.g. {'.txt', '.csv'}
    """
    root_path = Path(root_dir).resolve()
    output_path = Path(output_dir).resolve()

    allowed_extensions = {
        ext.lower() if ext.startswith(".") else f".{ext.lower()}"
        for ext in allowed_extensions
    }    
    
            
    data = list(filter(lambda f : f.is_file() and f.suffix.lower() in allowed_extensions,root_path.rglob("*")))
    print(*data,sep='\n')
    
    with ThreadPoolExecutor(max_workers=threads) as ex:
        _ = list(ex.map(worker, [(d,root_path,output_path) for d in data]))
        

    '''
    for file_path in root_path.rglob("*"):
        if not file_path.is_file():
            continue

        if file_path.suffix.lower() not in allowed_extensions:
            continue

        try:
            next(graphs_checked)
            accept,V = graphFilter(file_path)
            if accept:
                next(graphs_accepted)
                relative_path = file_path.relative_to(root_path)
                destination = output_path / relative_path

                destination.parent.mkdir(parents=True, exist_ok=True)
                
                #shutil.copy2(file_path, destination)
                #print(f"Copied: {file_path} -> {destination}")
                
                destination = destination.with_suffix('.txt')
                graph_format = 'std1'
                writeGraphToFile(V,destination,graph_format)
                print(f'Wrote graph from {file_path} to {destination} in {graph_format} format\n\n')
            else:
                print(f"\t Skipped by filter: {file_path}")

        except Exception as e:
            print(f"Error processing {file_path}: {e}")
    '''

    global graphs_checked, graphs_accepted
    print(f'\n\n{graphs_checked=} {graphs_accepted=}')

if __name__ == "__main__":
    ROOT_DIR = "GRAPH_INSTANCES"
    OUTPUT_DIR = os.path.join('.','FILTERED_AND_COPIED_GRAPH_INSTANCES')
    #ALLOWED_EXTENSIONS = {'.mtx'}
    ALLOWED_EXTENSIONS = {'.mtx', '.edges'}
    
    start = time.time()
    filter_and_copy_files(ROOT_DIR, OUTPUT_DIR, ALLOWED_EXTENSIONS)
    end = time.time()
    print(f"\n\nTime elapsed: {end - start:.6f} seconds")