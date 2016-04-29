""" This program makes the bridge between a meshbee network and a http server """

import io
import time
import serial
import urllib2

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
        self.meshbee.write(unicode('+++\r'))
        self.meshbee.flush()
        result = self.meshbee.readline()
        if "Error, invalid command" in result or "Enter AT Mode" in result:
            self.current_mode = "AT"
            self.meshbee.readline()
            return True
        return False

    def to_data_mode(self):
        """to_data_mode Got to DATA mode"""
        if self.current_mode == "DATA":
            return True
        if self.current_mode == "":
            print("UNKNOWN MODE in to_data_mode")
            return False
        self.meshbee.write(unicode('ATDT\r'))
        self.meshbee.flush()
        result = self.meshbee.readline()
        if "Enter Data Mode" in result:
            result = self.meshbee.readline()
            if "OK" in result:
                self.meshbee.readline()
                self.current_mode = "DATA"
                return True
        return False

    def print_information(self):
        """print_information Print the information of the node"""
        if not self.check_at_mode():
            return
        result = []
        self.meshbee.write(unicode('ATIF\r'))
        self.meshbee.flush()
        reading = self.meshbee.readline()
        while reading != "":
            result.append(reading)
            reading = self.meshbee.readline()
        print("INFORMATION ABOUT THE NODE")
        print("".join(result))

    def print_nodes(self):
        """print_nodes Print all the nodes in the network"""
        if not self.check_at_mode():
            return ""
        self.meshbee.write(unicode('ATLA\r'))
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
            return "NONODE"
        else:
            if not "Node resp" in reading:
                print("Problem Node resp")
            else:
                reading = self.meshbee.readline()
                while reading != "":
                    result.append(reading[3:])
                    reading = self.meshbee.readline()
                print("List of nodes :")
                nodes = "".join(result)
                print(nodes)
                return nodes
        return ""

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
        self.meshbee.write(unicode(message))
        self.meshbee.flush()

    def read_sensors(self):
        """readSensors read a line from a sensor"""
        if self.current_mode != "DATA":
            if self.current_mode != "AT":
                if not self.to_at_mode():
                    print("Problem going to AT mode")
                    return
            if not self.to_data_mode():
                print("Problem going to data mode")
                return
        reading = self.meshbee.readline()
        if len(reading) < 20:
            return
        data_type = reading[0:4]
        id_sensor = reading[4:4+16] #The mac address
        data = reading[4+16:]
        tosend = "http://localhost:9000/measuredata?id=" + id_sensor
        tosend = tosend + "&dataType=" + data_type  + "&data=" + data
        print("Sending a read_sensors")
        f = urllib2.urlopen(tosend)
        f.close()

def main():
    """main The main loop"""
    last_time = time.time()
    bee = MeshBee()
    while True:
        if time.time() - last_time > 30:
            print("Read node")
            nodes = bee.print_nodes()
            nodes = nodes.replace("\r", "")
            nodes = nodes.replace("\n", "")
            nodes = nodes.replace(" ", "")
            if nodes != "":
		print("Sending update")
		urllib2.urlopen("http://localhost:9000/updatesensors?s=" + nodes)
            last_time = time.time()
        bee.read_sensors()
    return 0

if __name__ == "__main__":
    main()
