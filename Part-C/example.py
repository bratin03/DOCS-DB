# Import the LSMTree class from lsm_tree.py
from src.lsm_tree import LSMTree

def main():
    # Initialize the LSM Tree object, providing the correct path to the shared library
    lsm_tree = LSMTree(lib_path='./build/liblsm_tree_wrapper.so')

    # Insert or update key-value pairs in the LSM Tree
    lsm_tree.put("key1", "value1")
    lsm_tree.put("key2", "value2")

    # Retrieve values
    print(f"key1: {lsm_tree.get('key1')}")
    print(f"key2: {lsm_tree.get('key2')}")

    # Remove a key-value pair
    lsm_tree.remove("key1")
    print(f"key1 after removal: {lsm_tree.get('key1')}")

if __name__ == "__main__":
    main()
