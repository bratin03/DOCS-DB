# /*
# Student Information:
# 1. Bratin Mondal - 21CS10016
# 2. Swarnabh Mandal - 21CS10068
# 3. Somya Kumar - 21CS30050

# Deparment of Computer Science and Engineering
# Indian Institute of Technology, Kharagpur
# */


import socket

class RESPClient:
    """
    @brief A client for communicating with a Redis server using the RESP protocol.
    
    This class handles establishing a connection, sending and receiving data,
    and formatting commands and responses according to the RESP protocol.

    @details The RESP protocol is used for communication between clients and servers
    in Redis-like systems. The RESPClient allows the client to send commands such as
    SET, GET, and DEL to a Redis server and receive responses. This class abstracts
    the communication logic and ensures that data is correctly formatted according to
    the RESP protocol.

    Attributes:
        host (str): The Redis server hostname (default is '127.0.0.1').
        port (int): The Redis server port (default is 6379).
        sock (socket.socket): The socket object used to communicate with the Redis server.
    """

    def __init__(self, host='127.0.0.1', port=6379):
        """
        @brief Initializes the RESPClient with the specified host and port.

        @param host The Redis server hostname (default is '127.0.0.1').
        @param port The Redis server port (default is 6379).
        """
        self.host = host
        self.port = port
        self.sock = None

    def connect(self):
        """
        @brief Establishes a connection to the Redis server.

        @throws ConnectionError If the connection to the Redis server fails.
        """
        try:
            self.sock = socket.create_connection((self.host, self.port))
            print(f"Connected to {self.host}:{self.port}")
        except socket.error as e:
            print(f"Failed to connect to {self.host}:{self.port}, Error: {e}")
            raise

    def send(self, data):
        """
        @brief Sends data to the server.

        @param data The data to send, as a string.
        
        @throws ConnectionError If the socket is not connected.
        """
        if not self.sock:
            raise ConnectionError("Socket is not connected.")
        self.sock.sendall(data.encode('utf-8'))

    def receive(self):
        """
        @brief Receives data from the server.

        @return The server's response, as a string.
        
        @throws ConnectionError If the socket is not connected.
        """
        if not self.sock:
            raise ConnectionError("Socket is not connected.")
        response = self.sock.recv(4096)
        return response.decode('utf-8')

    def close(self):
        """
        @brief Closes the connection to the server.
        """
        if self.sock:
            self.sock.close()
            self.sock = None
            print("Connection closed.")

    def send_command(self, *args):
        """
        @brief Sends a command to the Redis server using the RESP protocol.

        @param args The command and arguments to send to Redis.
        @return The server's response.
        """
        # Format command in RESP
        resp_command = self.format_resp(*args)
        self.send(resp_command)
        return self.receive()
    
    def parse_response(self, response, type):
        """
        @brief Parses the response from the Redis server.

        @param response The response from the server.
        @param type The type of command that was executed ("GET", "SET", etc.).
        @return The parsed response based on the command type.
        """
        if type == "GET":
            if response == "$-1\r\n":
                return None
            else:
                response = response.split("\r\n")
                return response[1]
        else:
            return response[1:-2]

    @staticmethod
    def format_resp(*args):
        """
        @brief Formats the command and its arguments according to the RESP protocol.

        @param args The command and arguments to format.
        @return The command formatted as a RESP string.
        """
        resp = f"*{len(args)}\r\n"
        for arg in args:
            arg_str = str(arg)
            resp += f"${len(arg_str)}\r\n{arg_str}\r\n"
        return resp


if __name__ == "__main__":
    """
    @brief Example usage of the RESPClient class.
    
    This script connects to the Redis server and sends commands iteratively
    until the user inputs 'exit' or 'EXIT'.
    """
    client = RESPClient(host='192.168.122.32')
    try:
        client.connect()

        while True:
            # Take user input for commands
            user_input = input("Enter command (or type 'exit' to quit): ").strip()

            # Exit condition
            if user_input.lower() == "exit":
                print("Exiting...")
                break

            # Split the input into command and arguments
            command_parts = user_input.split(" ", 2)
            command = command_parts[0].upper()

            if command == "SET" and len(command_parts) == 3:
                # Process SET command
                key = command_parts[1]
                value = command_parts[2]

                # Remove surrounding quotes from value if present
                if value.startswith('"') and value.endswith('"'):
                    value = value[1:-1]

                response = client.send_command(command, key, value)
                print("Response:", client.parse_response(response, "SET"))

            elif command == "GET" and len(command_parts) == 2:
                # Process GET command
                key = command_parts[1]
                response = client.send_command(command, key)
                print("Response:", client.parse_response(response, "GET"))

            elif command == "DEL" and len(command_parts) == 2:
                # Process DEL command
                key = command_parts[1]
                response = client.send_command(command, key)
                print("Response:", client.parse_response(response, "DEL"))

            else:
                print("Invalid command or format. Please use SET, GET, or DEL.")
                
    finally:
        client.close()
