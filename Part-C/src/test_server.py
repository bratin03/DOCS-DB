import unittest
import asyncio
from server import handle_set, handle_get, handle_del, parse_resp, build_resp, build_resp_get, build_error
from lsm_tree import LSMTree


class TestServer(unittest.TestCase):
    """Test server commands and responses."""
    
    def setUp(self):
        """Set up the test environment."""
        print("\n" + "="*50)
        print("Setting up the test environment...")
        self.lsm_tree = LSMTree()
        print("Test environment set up successfully.")
        print("="*50 + "\n")

    def tearDown(self):
        """Clean up after each test."""
        print("\n" + "="*50)
        print("Cleaning up the test environment...")
        del self.lsm_tree
        print("Test environment cleaned up successfully.")
        print("="*50 + "\n")

    def test_parse_resp_valid(self):
        """Test parsing a valid RESP-2 message."""
        print("\n" + "-"*50)
        print("Running test: test_parse_resp_valid")
        message = b"*3\r\n$3\r\nSET\r\n$4\r\nkey1\r\n$6\r\nvalue1\r\n"
        print(f"Message to parse: {message}")
        command, args = parse_resp(message)
        print(f"Parsed command: '{command}', Parsed args: {args}")
        self.assertEqual(command, "SET")
        self.assertEqual(args, ["key1", "value1"])
        print("Test passed: Command and arguments parsed correctly.")
        print("-"*50 + "\n")

    def test_handle_set(self):
        """Test handling the SET command."""
        print("\n" + "-"*50)
        print("Running test: test_handle_set")
        response = asyncio.run(handle_set("key1", "value1", self.lsm_tree))
        print(f"Response from handle_set: '{response}'")
        self.assertEqual(response, "OK")
        
        # Check if the key is set correctly
        value = self.lsm_tree.get("key1")
        print(f"Value for 'key1' in LSMTree: '{value}'")
        self.assertEqual(value, "value1")
        print("Test passed: 'key1' set with value 'value1'.")
        print("-"*50 + "\n")

    def test_handle_get_existing_key(self):
        """Test handling the GET command for an existing key."""
        print("\n" + "-"*50)
        print("Running test: test_handle_get_existing_key")
        self.lsm_tree.put("key1", "value1")  # Set a key-value pair
        print("Inserted key1 with value1 into LSMTree.")
        flag, response = asyncio.run(handle_get("key1", self.lsm_tree))
        print(f"GET response for 'key1': flag={flag}, response={response}")
        self.assertEqual(flag, 1)
        self.assertEqual(response, "value1")
        print("Test passed: Correct value returned for existing key 'key1'.")
        print("-"*50 + "\n")

    def test_handle_get_non_existing_key(self):
        """Test handling the GET command for a non-existing key."""
        print("\n" + "-"*50)
        print("Running test: test_handle_get_non_existing_key")
        flag, response = asyncio.run(handle_get("non_existing_key", self.lsm_tree))
        print(f"GET response for 'non_existing_key': flag={flag}, response={response}")
        self.assertEqual(flag, 0)
        self.assertIsNone(response)
        print("Test passed: Correct response for non-existing key.")
        print("-"*50 + "\n")

    def test_handle_del_existing_key(self):
        """Test handling the DEL command for an existing key."""
        print("\n" + "-"*50)
        print("Running test: test_handle_del_existing_key")
        self.lsm_tree.put("key1", "value1")  # Set a key-value pair
        print("Inserted key1 with value1 into LSMTree.")
        response = asyncio.run(handle_del("key1", self.lsm_tree))
        print(f"DEL response for 'key1': {response}")
        self.assertEqual(response, "OK")
        self.assertIsNone(self.lsm_tree.get("key1"))  # Ensure the key is deleted
        print("Test passed: 'key1' deleted successfully.")
        print("-"*50 + "\n")

    def test_handle_del_non_existing_key(self):
        """Test handling the DEL command for a non-existing key."""
        print("\n" + "-"*50)
        print("Running test: test_handle_del_non_existing_key")
        response = asyncio.run(handle_del("non_existing_key", self.lsm_tree))
        print(f"DEL response for 'non_existing_key': {response}")
        self.assertIsNone(response)
        print("Test passed: Correct response for non-existing key.")
        print("-"*50 + "\n")

    def test_build_resp(self):
        """Test building a RESP-2 simple string response."""
        print("\n" + "-"*50)
        print("Running test: test_build_resp")
        response = build_resp("OK")
        print(f"Built RESP response: {response}")
        self.assertEqual(response, b"+OK\r\n")
        print("Test passed: RESP response built correctly.")
        print("-"*50 + "\n")

    def test_build_resp_get(self):
        """Test building a RESP-2 bulk string response."""
        print("\n" + "-"*50)
        print("Running test: test_build_resp_get")
        response = build_resp_get("value1")
        print(f"Built RESP GET response: {response}")
        self.assertEqual(response, b"$6\r\nvalue1\r\n")
        print("Test passed: RESP GET response built correctly.")
        print("-"*50 + "\n")

    def test_build_error(self):
        """Test building a RESP-2 error response."""
        print("\n" + "-"*50)
        print("Running test: test_build_error")
        response = build_error("Invalid command")
        print(f"Built RESP ERROR response: {response}")
        self.assertEqual(response, b"-ERR Invalid command\r\n")
        print("Test passed: RESP ERROR response built correctly.")
        print("-"*50 + "\n")


if __name__ == '__main__':
    unittest.main()
