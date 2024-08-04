import simple_pb2
import hdlc
from test import print_byte
message = simple_pb2.SimpleMessage()
message.lucky_number = 121
message.unlucky_number = 1
serialized_message = message.SerializeToString()
print_byte(serialized_message)
encoded = hdlc.hdlc_encode(serialized_message)
print_byte(encoded)
decoded = hdlc.hdlc_decode(encoded)
print_byte(decoded)
print(decoded == serialized_message)

message = simple_pb2.SimpleMessage()
message.ParseFromString(bytes(decoded))
print(message)