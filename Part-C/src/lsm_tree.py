import ctypes
from ctypes import c_char_p


class LSMTree:
    def __init__(self, lib_path='../build/liblsm_tree_wrapper.so'):
        """
        Initializes the LSMTree object by loading the shared library
        and setting up function signatures.
        """
        self.lsm_tree_lib = ctypes.CDLL(lib_path)

        # Define argument and return types for each function
        self.lsm_tree_lib.lsm_tree_create.restype = ctypes.POINTER(ctypes.c_void_p)
        self.lsm_tree_lib.lsm_tree_put.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p, c_char_p]
        self.lsm_tree_lib.lsm_tree_get.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p]
        self.lsm_tree_lib.lsm_tree_get.restype = c_char_p
        self.lsm_tree_lib.lsm_tree_remove.argtypes = [ctypes.POINTER(ctypes.c_void_p), c_char_p]
        self.lsm_tree_lib.lsm_tree_destroy.argtypes = [ctypes.POINTER(ctypes.c_void_p)]

        # Create a new instance of the LSM tree
        self.tree = self.lsm_tree_lib.lsm_tree_create()

    def put(self, key: str, value: str):
        """
        Inserts or updates a key-value pair in the LSM tree.
        """
        self.lsm_tree_lib.lsm_tree_put(self.tree, key.encode('utf-8'), value.encode('utf-8'))

    def get(self, key: str) -> str:
        """
        Retrieves the value associated with the given key from the LSM tree.
        """
        result = self.lsm_tree_lib.lsm_tree_get(self.tree, key.encode('utf-8'))
        return result.decode('utf-8') if result else None

    def remove(self, key: str):
        """
        Removes the key-value pair from the LSM tree.
        """
        self.lsm_tree_lib.lsm_tree_remove(self.tree, key.encode('utf-8'))

    def __del__(self):
        """
        Destructor to clean up and destroy the LSM tree instance.
        """
        self.lsm_tree_lib.lsm_tree_destroy(self.tree)


# Example usage of the LSMTree class:
if __name__ == "__main__":
    # Create an instance of the LSMTree
    lsm_tree = LSMTree()

    # Insert key-value pairs
    lsm_tree.put("key1", "value1")
    lsm_tree.put("key2", "value2")

    # Retrieve values
    print(f"key1: {lsm_tree.get('key1')}")
    print(f"key2: {lsm_tree.get('key2')}")

    # Remove a key-value pair
    lsm_tree.remove("key1")
    print(f"key1 after removal: {lsm_tree.get('key1')}")

    # Clean up will be done automatically when the object is deleted
