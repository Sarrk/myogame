import socket
import time
import datetime
import struct
from threading import Thread
import sys

class Kalman:
    P = [[0, 0], [0, 0]]

    def __init__(self):
       # self.P = [[0 for x in range(2)] for y in range [2]]
        self.Q_angle = 0.001 #0.001
        self.Q_bias = 0.003 #0.003
        self.R_measure = 0.03 # 0.03

        self.angle = 0.0 # Reket the angle
        self.bias = 0.0 # Reset bias
        self.rate = 0.0

        self.P[0][0] = 0.0 
        self.P[0][1] = 0.0
        self.P[1][0] = 0.0
        self.P[1][1] = 0.0

    def getAngle(self, newAngle, newRate, dt):
        self.rate = newRate - self.bias
        self.angle += dt * self.rate

        
        # Step 2 */
        self.P[0][0] += dt * (dt * self.P[1][1] - self.P[0][1] - self.P[1][0] + self.Q_angle)
        self.P[0][1] -= dt * self.P[1][1]
        self.P[1][0] -= dt * self.P[1][1]
        self.P[1][1] += self.Q_bias * dt

        #/* Step 4 */
        S = self.P[0][0] + self.R_measure # Estimate error
        #/* Step 5 */
        K = [0, 0]
        K[0] = self.P[0][0] / S
        K[1] = self.P[1][0] / S

       
        #/* Step 3 */
        y = newAngle - self.angle
        #/* Step 6 */
        self.angle += K[0] * y
        self.bias += K[1] * y

        #Update the error covariance
        P00_temp = self.P[0][0]
        P01_temp = self.P[0][1]

        self.P[0][0] -= K[0] * P00_temp
        self.P[0][1] -= K[0] * P01_temp
        self.P[1][0] -= K[1] * P00_temp
        self.P[1][1] -= K[1] * P01_temp

        return self.angle

    def setAngle(self,  newAngle):
        self.angle = newAngle 

    def getRate(self):
        return self.rate # Return the unbiased rate

    #/* These are used to tune the Kalman filter */
    def setQangle(self, angle):
        self.Q_angle = angle; 

    def setQbias(self, bias): 
        self.Q_bias = bias

    def setRmeasure(self, measure):
        self.R_measure = measure

    def getQangle(self):
        return self.Q_angle
        
    def getQbias(self):
        return self.Q_bias

    def getRmeasure(self):
        return self.R_measure
