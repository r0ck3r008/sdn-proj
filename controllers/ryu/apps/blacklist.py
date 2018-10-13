import globals_defs as gl

def read_blacklist_file():
    
    with open(gt.blacklist_path, 'r') as f:
        gt.blacklist=f.read().strip().split('\n')

