#Investigate the IPC on ROS

##Introduction
ROS (robot operating system) is a framework designed for controlling different components of robots. This framework consists of many independent nodes controlled by the masters. Nodes communicate with each other through passing and receiving messages, majorly using publisher-subscriber semantics or request-reply semantics. In the publisher-subscriber semantics, message is published by nodes to a certain topics which is then subscribed by other node, while in the request-reply semantics, the communication is implemented in a service defined by a pair of messages: request and reply. This peer-to-peer network in a given ROS is often described in a computational graph. Master which contains all the registration information of nodes acts as the name service for the communication of the nodes in the computational graph. This peer-to-peer design enables individual execution and isolated processing; those individual processes can be further packaged and unpackaged for a given robotic design.

##Problems to address
We aim to finally understand the mechanism of inter-process communication (IPC) in ROS in this research project, including (1) how is IPC framed and implemented in ROS? (2) What are the major drawbacks of current IPC mechanism in ROS? Specifically, like, what is bottleneck of message communications between nodes?

##Motivation
ROS facilitates the development of robot applications; however, it has several limitations on the performance. Understand and improve the performance of ROS can help on diversify areas which depends on ROS including robot research, robot product and education.

##Goal
Demonstrate new potential solutions to improve the performance of IPC in ROS with little to no effect on the safety.
