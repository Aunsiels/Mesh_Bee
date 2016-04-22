""" This program makes the bridge between a meshbee network and a http server """

import io
import serial

class MeshBee:
    """MeshBee: To communicate with a MeshBee"""

    def __init__(self):
        meshbee_serial = serial.Serial('/dev/ttyS0', 115200, timeout=1)
        self.meshbee = io.TextIOWrapper(io.BufferedRWPair(meshbee_serial, meshbee_serial))

        self.current_mode = ""

    def check_at_mode(self):
        """check_at_mode Check if we are in at mode"""
        if self.current_mode != "AT":
            print("Going to AT mode...")
            return self.to_at_mode()
        return True

    def to_at_mode(self):
        """to_at_mode Go to AT mode"""
        self.meshbee.write(b'+++\r')
        self.meshbee.flush()
        result = self.meshbee.readline()
        if result != "" and result != "":
            return False
        self.current_mode = "AT"
        return True

    def to_data_mode(self):
        """to_data_mode Got to DATA mode"""
        if self.current_mode == "DATA":
            return True
        if self.current_mode == "":
            print("UNKNOWN MODE in to_data_mode")
            return False
        self.meshbee.write(b'ATDT\r')
        self.meshbee.flush()
        result = self.meshbee.readline()
        if result != "" and result != "":
            return False
        self.current_mode = "DATA"
        return True

    def print_information(self):
        """print_information Print the information of the node"""
        if not self.check_at_mode():
            return
        result = []
        self.meshbee.write(b'ATIF\r')
        self.meshbee.flush()
        reading = self.meshbee.readline()
        while reading != "":
            result.append(reading)
        print("INFORMATION ABOUT THE NODE")
        print("".join(result))

    def print_nodes(self):
        """print_nodes Print all the nodes in the network"""
        if not self.check_at_mode():
            return
        self.meshbee.write(b'ATLA\r')
        self.meshbee.flush()
        reading = self.meshbee.readline()
        result = []
        if not "The request has been sent" in reading:
            print("Problem first line")
        reading = self.meshbee.readline()
        if not "Waiting for response..." in reading:
            print("Problem second line")
        reading = self.meshbee.readline()
        if not "OK" in reading:
            print("Problem third line")
        reading = self.meshbee.readline()
        reading = self.meshbee.readline()
        if reading == "":
            print("No node detected on the network")
        else:
            if not "Node resp" in reading:
                print("Problem Node resp")
            else:
                reading = self.meshbee.readline()
                while reading != "":
                    result.append(reading[3:])
                    reading = self.meshbee.readline()
                print("List of nodes :")
                print("".join(result))

    def write(self, message):
        """write Write data

        :param message: the data to send
        """
        if self.current_mode != "DATA":
            if self.current_mode != "AT":
                if not self.to_at_mode():
                    print("Problem going to AT mode")
                    return
            if not self.to_data_mode():
                print("Problem going to data mode")
                return
        self.meshbee.write(message)



def main():
    """main The main loop"""
    return 0

if __name__ == "__main__":
    main()
