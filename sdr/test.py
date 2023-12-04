import pyrtlsdrlib

# On another machine (typically)
client = pyrtlsdrlib.RtlSdrTcpClient(hostname='192.168.137.203', port=12345)
client.center_freq = 2e6
data = client.read_samples()