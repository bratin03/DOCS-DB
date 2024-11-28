import socket

class RESPClient:
    def __init__(self, host='127.0.0.1', port=6379):
        self.host = host
        self.port = port
        self.sock = None

    def connect(self):
        """Establish a connection to the Redis server."""
        try:
            self.sock = socket.create_connection((self.host, self.port))
            print(f"Connected to {self.host}:{self.port}")
        except socket.error as e:
            print(f"Failed to connect to {self.host}:{self.port}, Error: {e}")
            raise

    def send(self, data):
        """Send data to the server."""
        if not self.sock:
            raise ConnectionError("Socket is not connected.")
        self.sock.sendall(data.encode('utf-8'))

    def receive(self):
        """Receive data from the server."""
        if not self.sock:
            raise ConnectionError("Socket is not connected.")
        response = self.sock.recv(4096)
        return response.decode('utf-8')

    def close(self):
        """Close the connection."""
        if self.sock:
            self.sock.close()
            self.sock = None
            print("Connection closed.")

    def send_command(self, *args):
        """
        Send a command to the Redis server using the RESP protocol.
        :param args: Command and arguments to send to Redis.
        """
        # Format command in RESP
        resp_command = self.format_resp(*args)
        self.send(resp_command)
        return self.receive()
    
    def parse_response(self, response, type):
        

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
        Format the command and its arguments in RESP protocol.
        :param args: Command and arguments to format.
        :return: RESP formatted string.
        """
        resp = f"*{len(args)}\r\n"
        for arg in args:
            arg_str = str(arg)
            resp += f"${len(arg_str)}\r\n{arg_str}\r\n"
        return resp


# Example usage
if __name__ == "__main__":
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
