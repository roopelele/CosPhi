import socket
import time

def main():
    #init TCP Socket
    TCP_IP = 'localhost'
    TCP_PORT = 6666
    BUFFER_SIZE = 1024
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))

    print("connected to the CosPhi- Topics are Publishing....")
    while True:
        payload = s.recv(BUFFER_SIZE).decode()
        robots = payload.split('\n')
        for i in range(len(robots)-2):
            _, id, x, y, angle, t = robots[i+1].split()
            timestamp = time.localtime(int(t) / 1000000.0)
            print(f"[{time.strftime('%H:%M:%S', timestamp)}]: {id}: ({x}, {y}), {angle}'")

main()
