import socket
import time

#import rospy
from std_msgs.msg import String


def main():
    # create rospy publisher
    #pub = rospy.Publisher('swarmcon', String, queue_size=10)
    #rospy.init_node('swarmcon')

    # init TCP Socket
    TCP_IP = 'localhost'
    TCP_PORT = 6666
    BUFFER_SIZE = 1024
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect((TCP_IP, TCP_PORT))

    print("connected to the CosPhi- Topics are Publishing....")
    #while not rospy.is_shutdown():
    while True:
        payload = s.recv(BUFFER_SIZE).decode()
        robots = payload.split('\n')
        msg = ""
        for i in range(len(robots)-2):
            _, id, x, y, angle, t = robots[i+1].split()
            #msg += f"{id}, {x}, {y}, {angle}"
            timestamp = time.localtime(int(t) / 1000000.0)
            print(f"[{time.strftime('%H:%M:%S', timestamp)}]: {id}: ({x}, {y}), {angle}'")
        #pub.publish(msg)


main()
