import asyncio
import uvloop
from multiprocessing import cpu_count
from src.lsm_tree import LSMTree

# Use uvloop for performance
asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

lsm_tree = None

# RESP-2 protocol parsing

def parse_resp(message):
    """
    @brief Parse a RESP-2 message to extract command and arguments.

    This function parses a RESP-2 message and extracts the command and arguments
    from the message. RESP-2 protocol is used in Redis-like systems for communication.

    @param message The RESP message in bytes to be parsed.
    @return A tuple containing:
        - command (str): The command extracted from the message.
        - args (list of str): The arguments for the command extracted from the message.
    """
    if message[0] == ord('*'):
        parts = message.split(b'\r\n')
        command = parts[2].decode().upper()
        args = [arg.decode() for arg in parts[4::2]]
        return command, args
    return None, None

def build_resp(message):
    """
    @brief Build a RESP-2 response message.

    This function constructs a simple RESP-2 response message based on the input
    message. The format of the response will be a simple string prefixed with '+'.

    @param message The message to send.
    @return The encoded RESP-2 message in bytes.
    """
    return f"+{message}\r\n".encode()

def build_resp_get(message):
    """
    @brief Build a RESP-2 GET response message.

    This function constructs a RESP-2 GET response message, which includes the
    length of the message followed by the actual message content.

    @param message The message to send.
    @return The encoded GET response message in bytes.
    """
    return f"${len(message)}\r\n{message}\r\n".encode()

def build_error(message):
    """
    @brief Build a RESP-2 error message.

    This function constructs a RESP-2 error message, which includes an error
    message prefixed with '-ERR'.

    @param message The error message to send.
    @return The encoded error message in bytes.
    """
    return f"-ERR {message}\r\n".encode()

# Command Handlers

async def handle_set(key, value):
    """
    @brief Handle the 'SET' command to store a key-value pair in the LSM tree.

    This function processes the 'SET' command, stores the provided key-value pair
    in the LSM tree, and returns an acknowledgment response.

    @param key The key to store.
    @param value The value to associate with the key.
    @return A string indicating the status (e.g., "OK").
    """
    lsm_tree.put(key, value)
    return "OK"

async def handle_get(key):
    """
    @brief Handle the 'GET' command to retrieve the value for a key.

    This function processes the 'GET' command, retrieves the value for the
    provided key from the LSM tree, and returns the value along with a success flag.

    @param key The key to retrieve.
    @return A tuple containing:
        - flag (int): 1 if the key was found, 0 otherwise.
        - value (str or None): The value associated with the key, or None if not found.
    """
    val = lsm_tree.get(key)
    if val is not None:
        return 1, val
    return 0, None

async def handle_del(key):
    """
    @brief Handle the 'DEL' command to delete a key from the LSM tree.

    This function processes the 'DEL' command, deletes the specified key from
    the LSM tree if it exists, and returns a success status. If the key does not
    exist, the function returns None.

    @param key The key to delete.
    @return A string indicating the status or None if the key was not found.
    """
    val = lsm_tree.get(key)
    if val is None:
        return None
    lsm_tree.remove(key)
    return "OK"

async def handle_client(reader, writer):
    """
    @brief Handle a client connection, parsing and responding to commands.

    This function is responsible for handling client connections, reading requests,
    parsing the commands, and sending appropriate responses back to the client.
    
    It supports multiple commands, such as 'SET', 'GET', and 'DEL'.

    @param reader The asyncio StreamReader to read client data.
    @param writer The asyncio StreamWriter to send responses to the client.
    """
    addr = writer.get_extra_info('peername')
    print(f"New connection from {addr}")
    try:
        while True:
            data = await reader.read(4096)  # Increased buffer size
            if not data:
                break
            command, args = parse_resp(data)
            response = None

            # Handle various commands based on the parsed command
            if command == "SET" and len(args) == 2:
                response = await handle_set(args[0], args[1])
                writer.write(build_resp(response))
            elif command == "GET" and len(args) == 1:
                flag, response = await handle_get(args[0])
                if flag:
                    writer.write(build_resp_get(response))
                else:
                    writer.write(build_error("Key not found"))
            elif command == "DEL" and len(args) == 1:
                response = await handle_del(args[0])
                if response:
                    writer.write(build_resp(response))
                else:
                    writer.write(build_error("Key not found"))
            else:
                writer.write(build_error("Invalid command or arguments"))

            await writer.drain()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        print(f"Connection closed: {addr}")
        writer.close()
        await writer.wait_closed()

async def start_worker(host, port):
    """
    @brief Start a worker to handle client connections on a given host and port.

    This function creates and runs an asyncio-based server worker, which listens for
    client connections on the specified host and port. The worker will handle multiple
    client connections in an asynchronous non-blocking manner.

    @param host The host address to bind the server.
    @param port The port to bind the server.
    @return None
    """
    server = await asyncio.start_server(
        handle_client, host, port, reuse_port=True
    )  # Use SO_REUSEPORT
    addr = server.sockets[0].getsockname()
    print(f"Worker running on {addr}")
    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    """
    @brief Main entry point for the server, starting workers and setting up LSM tree.

    This is the main function where the server is initialized. It sets up the LSM tree,
    determines the number of worker processes based on available CPU cores, and starts
    the workers to handle incoming client connections on the specified host and port.
    """
    host = "127.0.0.1"
    port = 6379
    lsm_tree = LSMTree(lib_path='./build/liblsm_tree_wrapper.so')

    workers = cpu_count()  # Number of workers equals CPU cores
    processes = []

    # Start worker processes for handling client connections
    for _ in range(workers):
        pid = asyncio.run(start_worker(host, port))  # Spawn workers on the same port

    # Wait for all processes to complete
    for process in processes:
        process.join()
