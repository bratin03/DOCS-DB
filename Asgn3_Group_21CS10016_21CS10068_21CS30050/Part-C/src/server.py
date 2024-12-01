# /*
# Student Information:
# 1. Bratin Mondal - 21CS10016
# 2. Swarnabh Mandal - 21CS10068
# 3. Somya Kumar - 21CS30050

# Deparment of Computer Science and Engineering
# Indian Institute of Technology, Kharagpur
# */

"""
@file server.py

@brief A simple asyncio-based server for handling client connections and interacting with an LSM tree.
"""

import asyncio
import uvloop
from multiprocessing import cpu_count
from lsm_tree import LSMTree
import threading

# Use uvloop for better performance
asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

lsm_tree = None

# RESP-2 Utilities

"""
@brief Signal handler for Ctrl+C to gracefully exit the server after calling the destructor of lsm tree
@param sig The signal number.
@param frame The current stack frame.

"""


def signal_handler(sig, frame):

    print("\nClosing the server and cleaning up resources...")
    lsm_tree.__del__()

    import os

    os._exit(0)


"""
@brief Parse a RESP-2 message to extract command and arguments.
@param message The RESP-2 formatted message as bytes.
@return A tuple containing the command as a string and a list of arguments.
"""


def parse_resp(message):
    try:
        parts = message.split(b"\r\n")
        if parts[0][0] != ord("*"):
            raise ValueError("Invalid RESP format")
        arg_count = int(parts[0][1:])
        args = []
        index = 1
        for _ in range(arg_count):
            if parts[index][0] != ord("$"):
                raise ValueError("Invalid RESP bulk string format")
            length = int(parts[index][1:])
            index += 1
            args.append(parts[index].decode())
            index += 1
        command = args.pop(0).upper()
        return command, args
    except Exception as e:
        print(f"Error parsing RESP: {e}")
        return None, None


"""
@brief Build a RESP-2 simple string response.
@param message The message to send as a simple string response.
@return The encoded RESP-2 response as bytes.
"""


def build_resp(message):
    return f"+{message}\r\n".encode()


"""
@brief Build a RESP-2 bulk string response.
@param message The message to send as a bulk string response.
@return The encoded RESP-2 response as bytes.
"""


def build_resp_get(message):
    return f"${len(message)}\r\n{message}\r\n".encode()


"""
@brief Build a RESP-2 error response.
@param message The error message to send.
@return The encoded RESP-2 error response as bytes.
"""


def build_error(message):
    return f"-ERR {message}\r\n".encode()


# Command Handlers

"""
@brief Handle the 'SET' command to store a key-value pair in the LSM tree.
@param key The key to store.
@param value The value to associate with the key.
@return A string response indicating success.
"""


async def handle_set(key, value, lsm_tree=lsm_tree):
    lsm_tree.put(key, value)
    return "OK"


"""
@brief Handle the 'GET' command to retrieve the value for a key.
@param key The key to retrieve the value for.
@return A tuple containing a flag (1 if the key exists, 0 otherwise) and the value or None.
"""


async def handle_get(key, lsm_tree=lsm_tree):
    val = lsm_tree.get(key)
    if val is not None:
        return 1, val
    return 0, None


"""
@brief Handle the 'DEL' command to delete a key from the LSM tree.
@param key The key to delete.
@return A string response indicating success or None if the key does not exist.
"""


async def handle_del(key, lsm_tree=lsm_tree):
    val = lsm_tree.get(key)
    if val is None:
        return None
    lsm_tree.remove(key)
    return "OK"


"""
@brief Handle a client connection, parsing and responding to commands.
@param reader The asyncio StreamReader for reading data from the client.
@param writer The asyncio StreamWriter for sending data to the client.
"""


async def handle_client(reader, writer):
    addr = writer.get_extra_info("peername")
    # print(f"New connection from {addr}")
    try:
        while True:
            data = await reader.read(4096)
            if not data:
                break
            command, args = parse_resp(data)
            if not command:
                writer.write(build_error("Invalid RESP format"))
                await writer.drain()
                continue

            # Handle commands
            response = None
            if command == "SET" and len(args) == 2:
                response = await handle_set(args[0], args[1], lsm_tree)
                writer.write(build_resp(response))
            elif command == "GET" and len(args) == 1:
                flag, response = await handle_get(args[0], lsm_tree)
                if flag:
                    writer.write(build_resp_get(response))
                else:
                    writer.write(build_error("Key not found"))
            elif command == "DEL" and len(args) == 1:
                response = await handle_del(args[0], lsm_tree)
                if response:
                    writer.write(build_resp(response))
                else:
                    writer.write(build_error("Key not found"))
            elif command == "PING":
                writer.write(build_resp("PONG"))
            elif command == "CONFIG":
                writer.write(build_error("CONFIG not supported"))
            else:
                writer.write(build_error("Invalid command or arguments"))

            await writer.drain()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        # print(f"Connection closed: {addr}")
        writer.close()
        await writer.wait_closed()


"""
@brief Start a worker to handle client connections on a given host and port.
@param host The host to bind the server to.
@param port The port to bind the server to.
"""


async def start_worker(host, port):
    server = await asyncio.start_server(handle_client, host, port,  reuse_port=True)
    addr = server.sockets[0].getsockname()
    print(f"Worker running on {addr}")
    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    import signal

    signal.signal(signal.SIGINT, signal_handler)
    host = "192.168.122.32"
    port = 6379
    lsm_tree = LSMTree()

    NUM_THREADS = cpu_count() // 3
    NUM_THREADS = 1 if NUM_THREADS == 0 else NUM_THREADS

    # Start event loop concurrently in 2 threads to handle multiple clients on same port
    threads = [
        threading.Thread(target=asyncio.run, args=(start_worker(host, port),))
        for _ in range(NUM_THREADS)
    ]
    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
