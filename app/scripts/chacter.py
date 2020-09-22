"""
    Character.py
    Script to control Character
"""

import igeCore as core
from igeCore import figure
from igeCore.input.keyboard import KeyCode, Keyboard

import igeVmath as vmath
from igeScene import Script, SceneManager, Scene


STATUS_STAY = 0
STATUS_WALK = 1
STATUS_RUN = 2

STATE_MOTION = {STATUS_STAY:"Sapphiart@idle", STATUS_WALK:"Sapphiart@walk", STATUS_RUN:"Sapphiart@running"}

TRANSIT_TIME = 0.2

class Character(Script):
    def __init__(self, owner):
        super().__init__(owner)
        self.updated = False
        self.figureComp = None
        self.figure = None
        self.currentState = STATUS_STAY
        self.nextState = STATUS_STAY
        print("Character __init__!")

    def onStart(self):
        print("onStart")
        print(f'Owner: {self.owner}')
        self.figureComp = self.owner.getComponent("FigureComponent")
        if self.figureComp != None:
            print(f"Figure component: {self.figureComp}")
            self.figure = self.figureComp.figure
            if self.figure != None:
                print(f"Figure: {self.figureComp}")
                self.figure.connectAnimator(core.ANIMETION_SLOT_A0, STATE_MOTION[self.currentState])

    def changeStatus(self, status):
        print(f'changeStatus: {status}')
        if status != self.currentState and status != self.nextState:
            self.nextState = status
            self.figure.connectAnimator(core.ANIMETION_SLOT_A1, STATE_MOTION[status])
            self.transitTime = 0.0

    def transitMotion(self):
        if self.currentState != self.nextState:
            if self.transitTime >= TRANSIT_TIME:
                self.currentState = self.nextState
                self.figure.connectAnimator(core.ANIMETION_SLOT_A0, STATE_MOTION[self.currentState])
                self.figure.connectAnimator(core.ANIMETION_SLOT_A1)
                return

            self.transitTime += core.getElapsedTime()
            if self.transitTime > TRANSIT_TIME:
                self.transitTime = TRANSIT_TIME
            self.figure.setBlendingWeight(core.ANIMETION_PART_A, self.transitTime / TRANSIT_TIME)

    def onUpdate(self, dt):
        if not self.updated:
            print(f'onUpdate, dt = {dt}')
            print(f'owner, name = {self.owner.name}')
            self.updated = True

        self.transitMotion()

        scene = SceneManager.getInstance().currentScene
        camera = None
        if scene != None:
            # print(f"scene: {scene}")
            camera = scene.activeCamera

            if camera != None:
                # print(f"camera: {camera}")
                if core.isFingerPressed(1):
                    ori_x, ori_y = core.getFingerPosition(1)
                    print(f'ori_x: {ori_x}, ori_y: {ori_y}')

                moveVector = vmath.vec3(0.0, 0.0, 0.0)

                keyboard = Keyboard.instance()
                if keyboard is not None:
                    if keyboard.isPressed(KeyCode.KEY_W):
                        print('isPressed: W')
                        moveVector = vmath.vec3(0.0, 0.0, -10.0)
                        self.changeStatus(STATUS_RUN)
                    elif keyboard.isPressed(KeyCode.KEY_S):
                        print('isPressed: S')
                        moveVector = vmath.vec3(0.0, 0.0, 10.0)
                        self.changeStatus(STATUS_WALK)
                    elif keyboard.isPressed(KeyCode.KEY_A):
                        print('isPressed: A')
                        moveVector = vmath.vec3(-10.0, 0.0, 0.0)
                    elif keyboard.isPressed(KeyCode.KEY_D):
                        print('isPressed: D')
                        moveVector = vmath.vec3(10.0, 0.0, 0.0)

                # print(f'camera: {camera}')
                # viewmat = camera.viewInverseMatrix
                # if viewmat is not None:
                    # print(f'viewmat: {viewmat}')

                    # moveVector = vmath.vec3(viewMat * moveVector)

                    # print('isPressed: W')

                    # l = vmath.length(moveVector)
                    # if l > 20.0:
                    #     # self.gorlDir = vmath.normalize(moveVector)
                    #     self.changeStatus(STATUS_RUN)
                    # elif l > 5.0:
                    #     # self.gorlDir = vmath.normalize(moveVector)
                    #     self.changeStatus(STATUS_WALK)
                    # else:
                    #     self.changeStatus(STATUS_STAY)
