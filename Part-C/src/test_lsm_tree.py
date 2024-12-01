import unittest
from unittest.mock import patch
from server import LSMTree


class TestLSMTree(unittest.TestCase):
    """Test operations on the LSM Tree."""
    
    def setUp(self):
        """Set up an instance of the LSM tree before each test."""
        print("\n" + "="*50)
        print("Setting up the LSMTree instance for the test...")
        self.lsm_tree = LSMTree()
        print("LSMTree instance set up successfully.")
        print("="*50 + "\n")

    def tearDown(self):
        """Clean up after each test."""
        print("\n" + "="*50)
        print("Cleaning up the LSMTree instance...")
        del self.lsm_tree
        print("LSMTree instance cleaned up successfully.")
        print("="*50 + "\n")

    def test_put_and_get(self):
        """Test inserting and retrieving key-value pairs."""
        print("\n" + "-"*50)
        print("Running test: test_put_and_get")
        # Insert key-value pairs
        print("Inserting key-value pairs:")
        self.lsm_tree.put("key1", "value1")
        self.lsm_tree.put("key2", "value2")
        print("Inserted 'key1' with 'value1' and 'key2' with 'value2'.")

        # Retrieve values and assert correctness
        value1 = self.lsm_tree.get("key1")
        value2 = self.lsm_tree.get("key2")
        print(f"Retrieved value for 'key1': {value1}")
        print(f"Retrieved value for 'key2': {value2}")
        self.assertEqual(value1, "value1")
        self.assertEqual(value2, "value2")
        print("Test passed: Retrieved values match expected values.")
        print("-"*50 + "\n")

    def test_remove(self):
        """Test removing a key-value pair."""
        print("\n" + "-"*50)
        print("Running test: test_remove")
        # Insert a key-value pair
        self.lsm_tree.put("key1", "value1")
        print("Inserted 'key1' with 'value1'.")

        # Remove the key-value pair
        print("Removing 'key1'...")
        self.lsm_tree.remove("key1")
        print("Removed 'key1'.")

        # Try to retrieve the removed key
        value = self.lsm_tree.get("key1")
        print(f"Attempted retrieval of 'key1': {value}")
        self.assertIsNone(value, "Expected value to be None after removal")
        print("Test passed: 'key1' removed successfully and retrieval returned None.")
        print("-"*50 + "\n")

    def test_get_non_existing_key(self):
        """Test retrieving a non-existing key."""
        print("\n" + "-"*50)
        print("Running test: test_get_non_existing_key")
        # Try to get a key that doesn't exist
        print("Attempting to retrieve 'non_existing_key'...")
        value = self.lsm_tree.get("non_existing_key")
        print(f"Retrieved value for 'non_existing_key': {value}")
        self.assertIsNone(value, "Expected None for non-existing key")
        print("Test passed: Correctly returned None for non-existing key.")
        print("-"*50 + "\n")


if __name__ == '__main__':
    unittest.main()
