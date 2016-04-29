""" This program makes the bridge between a meshbee network and a http server """

import io
import time
import serial
import urllib2

class MeshBee:
    """MeshBee: To communicate with a MeshBee"""

    def __init__(self):
        self.meshbee_serial = serial.Serial('/dev/ttyS0', 115200, timeout=1)
        #self.meshbee = io.TextIOWrapper(io.BufferedRWPair(meshbee_serial, meshbee_serial))
        self.meshbee = io.BufferedRWPair(self.meshbee_serial, self.meshbee_serial)
        # Will contain mac addresses read when requesting the topologie
        self.mac_adresses = []
        self.current_mode = ""

    def check_at_mode(self):
        """check_at_mode Check if we are in at mode"""
        if self.current_mode != "AT":
            print("Going to AT mode...")
            return self.to_at_mode()
        return True

    def to_at_mode(self):
        """to_at_mode Go to AT mode"""
        # Here I throw away every message in the buffer
        self.meshbee_serial.timeout=0
        while self.meshbee.readline() != "":
            continue
        self.meshbee_serial.timeout=1
        self.meshbee.write(unicode('+++\r'))
        self.meshbee.flush()
        result = self.meshbee.readline()
        if "Error, invalid command" in result or "Enter AT Mode" in result:
            self.current_mode = "AT"
            self.meshbee.readline()
            return True
        print(result)
        return False

    def to_data_mode(self):
        """to_data_mode Go to DATA mode"""
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

    def to_api_mode(self):
        """to_api_mode Go to API mode"""
	if self.current_mode == "API":
            return True
        if self.current_mode != "AT":
            self.to_at_mode()
        self.meshbee.write(unicode('ATAP\r'))
        self.meshbee.flush()
        result = self.meshbee.readline()
        if "Enter API Mode." in result:
            result = self.meshbee.readline()
            if "OK" in result:
                self.meshbee.readline()
                self.current_mode = "API"
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

    def decrypt_message(self,  reading):
        """decrypt_message decrypt a message coming from a sensor
        and send the information to the server.

        :param reading: the message to decrypt
        """
        data_type = reading[0:4]
        id_sensor = reading[4:4+16] #The mac address
        data = reading[4+16:]
        tosend = "http://localhost:9000/measuredata?id=" + id_sensor
        tosend = tosend + "&dataType=" + data_type  + "&data=" + data
        print("Sending a read_sensors")
        try:
            f = urllib2.urlopen(tosend)
            f.close()
        except urllib2.URLError:
            print("No connection with the server")
            return 

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
        decrypt_message(reading)

    def checksum(self, check, payload):
        """checksum Check the payload

        :param check the checksum
        :param payload the payload to check"""
        sum = 0
        for number in payload:
            sum += ord(number)
            sum %= 256
        return sum == check

    def request_topo(self):
        """request_topo request topology of the network"""
        # I create the frame yhich calls APLA = 0x42
        self.send_mac_addresses()
        self.mac_adresses = []
        packet = [chr(0x7e), chr(0x03), chr(0x08), chr(0xec), chr(0x00), chr(0x42), chr(0x2e)]
        self.meshbee.write(''.join(packet))
        self.meshbee.flush()

    def api_data_frame(self, payload):
        """api_data_frame Read a data frame

        :param payload The data frame"""
        frame_id = ord(payload[0])
        option = ord(payload[1])
        addr_short = map(lambda x: ord(x), payload[2:4])
        addr_long = map(lambda x : '{:02x}'.format(ord(x)), payload[4:12]) #MAC address
        #TODO Use this MAC address
        length = ord(payload[12])
	body = payload[13:13+length]
        print(addr_long, body)
        #TODO remove \r\n
        self.decrypt_message(body[:-2])

    def api_topo_frame(self, payload):
        """api_data_frame Read a data frame

        :param payload The topo frame"""
        link_quality = ord(payload[0])
        dbm = ord(payload[1]) * 256 + ord(payload[2])
        firmware_version = ord(payload[3]) * 256 + ord(payload[4])
        addr_short = map(lambda x: ord(x), payload[5:7])
        mac_low = map(lambda x : '{:02x}'.format(ord(x)), payload[7:11])
        mac_high = map(lambda x : '{:02x}'.format(ord(x)), payload[11:15])
        self.mac_adresses.append(''.join(mac_high) + ''.join(mac_low))
        print(dbm, firmware_version, addr_short, mac_high, mac_low)

    def api_read_frame(self):
        """api_read_frame Read a frame in API mode"""
	if self.current_mode != "API":
            print("Going to API mode...")
            if not self.to_api_mode():
                print("Problem going to API mode")
        # We read the API frame header
        first_byte = self.meshbee.read(1);
        if first_byte != chr(0x7E):
            if first_byte == chr(0x00):
                # It means with receive 4 0x00 as ACK
                self.meshbee.read(3)
                return
	    return
	length_payload = ord(self.meshbee.read(1))
        api_identifier = ord(self.meshbee.read(1))
        payload = self.meshbee.read(length_payload)
        checksum = ord(self.meshbee.read(1))
        if not self.checksum(checksum, payload):
            print("Problem of checksum")
            return
        if api_identifier == 0x02: # Data Frame
            self.api_data_frame(payload)
        elif api_identifier == 0x6B:
            self.api_topo_frame(payload)
        else:
            print("No api identifier found", api_identifier, payload)

    def send_mac_addresses(self):
        """"send_mac_addresses send all mac addresses read to the server"""
        to_send = ''.join(self.mac_adresses)
        print('Node read', to_send)
        if to_send:
		print("Sending update")
                try:     
		    f = urllib2.urlopen("http://localhost:9000/updatesensors?s=" + to_send)
                    f.close()
                except urllib2.URLError:
                    print("No connection with the server")
                    return 
        else:
                try:
	            f = urllib2.urlopen("http://localhost:9000/updatesensors?s=" + '0')
                    f.close()
                except urllib2.URLError:
                    print("No connection with the server")
                    return 
def main():
    """main The main loop"""
    last_time = time.time()
    bee = MeshBee()
    while True:
        if time.time() - last_time > 30:
            bee.request_topo()
            last_time = time.time()
        bee.api_read_frame()
    return 0

if __name__ == "__main__":
    main()
