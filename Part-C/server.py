import asyncio
import uvloop
import sys
from src.lsm_tree import LSMTree


# Set uvloop as the event loop
asyncio.set_event_loop_policy(uvloop.EventLoopPolicy())

# In-memory storage for the key-value store
data_store = {}
lsm_tree = None
# RESP-2 protocol parsing and response functions
def parse_resp(message):
    if message.startswith("*"):
        parts = message.split("\r\n")
        command = parts[2].upper()
        args = parts[4::2]
        return command, args
    return None, None

def build_resp(message):
    return f"+{message}\r\n"

def build_resp_get(message):
    return f"${len(message)}\r\n{message}\r\n"

def build_error(message):
    return f"-ERR {message}\r\n"

# Command Handlers
async def handle_set(key, value):
    lsm_tree.put(key, value)
    return "OK"

async def handle_get(key):
    val = lsm_tree.get(key)
    if val is not None:
        return 1, val
    else:
        return 0, None

async def handle_del(key):
    val = lsm_tree.get(key)
    if val is None:
        return None
    lsm_tree.remove(key)
    return "OK"     
    

# Server handler for RESP commands
async def handle_client(reader, writer):
    addr = writer.get_extra_info('peername')
    print(f"New connection from {addr}")

    try:
        while True:
            data = await reader.read(1024)
            if not data:
                break
            message = data.decode()
            command, args = parse_resp(message)

            if command == "SET" and len(args) == 2:
                response = await handle_set(args[0], args[1])
                writer.write(build_resp(response).encode())p
                
            elif command == "GET" and len(args) == 1:
                flag, response = await handle_get(args[0])
                if flag:
                    writer.write(build_resp_get(response).encode())
                else:
                    writer.write(build_error('Key not found').encode())
                    
            elif command == "DEL" and len(args) == 1:
                response = await handle_del(args[0])
                if response:
                    writer.write(build_resp(response).encode())
                else:
                    writer.write(build_error('Key not found').encode()) 
                          
            else:
                writer.write(build_error("Invalid command or arguments").encode())

            await writer.drain()
    except Exception as e:
        print(f"Error: {e}")
    finally:
        print(f"Connection closed: {addr}")
        writer.close()

async def main(host, port):
    server = await asyncio.start_server(handle_client, host, port)
    addr = server.sockets[0].getsockname()
    print(f"Server running on {addr}")
    async with server:
        await server.serve_forever()

if __name__ == "__main__":
    host = "127.0.0.1"
    port = 6379
    lsm_tree = LSMTree(lib_path='./build/liblsm_tree_wrapper.so')
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    asyncio.run(main(host, port))
