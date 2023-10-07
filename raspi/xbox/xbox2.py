import pygame
import time

def XboxInit():
    pygame.init()
    pygame.joystick.init()
#done=False

def XboxJoystick():
    done=False

    while (done != True):
        for event in pygame.event.get():  # User did something
            if event.type == pygame.QUIT:  # If user clicked close
                done = True  # Flag that we are done so we exit this loop
        joystick_count = pygame.joystick.get_count()
        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()
            axes = joystick.get_numaxes()
            print('================')
            for i in range(axes):
                axis = joystick.get_axis(i)
                print(axis)
def XboxJoystick2():
    done=False
    j=1
    while (done != True):
        time.sleep(2)
        for event in pygame.event.get():  # User did something
            if event.type == pygame.QUIT:  # If user clicked close
                done = True  # Flag that we are done so we exit this loop
        joystick_count = pygame.joystick.get_count()
        for i in range(joystick_count):
            joystick = pygame.joystick.Joystick(i)
            joystick.init()
            axes = joystick.get_numaxes()
            print('================')
            for i in range(axes):
                axis = joystick.get_axis(i)
                print(axis)
        j=j+1
        if j == 3:
            break

if __name__ == '__main__':
    XboxInit()
    #XboxJoystick()
    while 1:
        XboxJoystick2()

