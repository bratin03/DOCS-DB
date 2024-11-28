import asyncio
import sys

# In-memory storage for the key-value store
data_store = {}

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
def handle_set(key, value):
    data_store[key] = value
    return "OK"

def handle_get(key):
    if key in data_store:
        return 1,data_store[key]
    else:
        return 0,None

def handle_del(key):
    if key in data_store:
        del data_store[key]
        return "OK"     
    return None
    

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

            print(f"Received command: {command} with args: {args}")

            if command == "SET" and len(args) == 2:
                response = handle_set(args[0], args[1])
                print(build_resp(response).encode())
                writer.write(build_resp(response).encode())
                
            elif command == "GET" and len(args) == 1:
                flag,response = handle_get(args[0])
                if(flag):
                    writer.write(build_resp_get(response).encode())
                else:
                    writer.write(build_error('Key not found').encode())
                    
                    
            elif command == "DEL" and len(args) == 1:
                response = handle_del(args[0])
                if(response):
                    writer.write(build_resp(response).encode())
                else:
                    writer.write(build_error('Key not found').encode()) 
                          
            else:
                print(build_error("Invalid command or arguments").encode())
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
    if len(sys.argv) > 1:
        port = int(sys.argv[1])
    asyncio.run(main(host, port))