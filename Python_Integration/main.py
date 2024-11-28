import ctypes
from ctypes import c_char_p

# Load the shared library
lsm_tree_lib = ctypes.CDLL('./liblsm_tree_wrapper.so')  # Adjust path as necessary

# Define the argument and return types for each function (if needed)
lsm_tree_lib.lsm_tree_create.restype = ctypes.POINTER(ctypes.c_void_p)  # Pointer to lsm_tree instance
lsm_tree_lib.lsm_tree_put.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p, c_char_p]
lsm_tree_lib.lsm_tree_get.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p]
lsm_tree_lib.lsm_tree_get.restype = c_char_p
lsm_tree_lib.lsm_tree_remove.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p]

# Create an instance of the LSM Tree
tree = lsm_tree_lib.lsm_tree_create()

# Insert or update a key-value pair
lsm_tree_lib.lsm_tree_put(tree, b"key1", b"value1")

# Retrieve a value
result = lsm_tree_lib.lsm_tree_get(tree, b"key1")
print(f"Retrieved value: {result.decode()}")

# Remove a key-value pair
lsm_tree_lib.lsm_tree_remove(tree, b"key1")

# Clean up (destroy the LSM tree)
lsm_tree_lib.lsm_tree_destroy(tree)
