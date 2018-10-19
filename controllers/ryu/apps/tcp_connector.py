import multiprocessing as mt
import ctypes as ct
import global_defs as gt

def tcp_connector():
    gt.relay_lib.get_connection_back(ct.c_int(gt.server_sock))
