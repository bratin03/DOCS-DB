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


# Example usage
if __name__ == "__main__":
    """
    @brief Example usage of the RESPClient class.
    
    This script connects to the Redis server and sends commands to set, get,
    and delete a key, while parsing and printing the responses.
    """
    client = RESPClient()
    try:
        client.connect()

        # Set a key
        response = client.send_command("SET", "mykey", "myvalue")
        print("Response:", client.parse_response(response, "SET"))

        # Get the key's value
        response = client.send_command("GET", "mykey")
        print("Response:", client.parse_response(response, "GET"))

        # Delete the key
        response = client.send_command("DEL", "mykey")
        print("Response:", client.parse_response(response, "DEL"))

        # Get the key's value again
        response = client.send_command("GET", "mykey")
        print("Response:", client.parse_response(response, "GET"))
    finally:
        client.close()
