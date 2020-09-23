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

STATE_MOTION = {STATUS_STAY: "Sapphiart@idle",
                STATUS_WALK: "Sapphiart@walk", STATUS_RUN: "Sapphiart@running"}

TRANSIT_TIME = 0.2


class Character(Script):
    def __init__(self, owner):
        super().__init__(owner)
        self.figure = None
        self.currentState = STATUS_STAY
        self.nextState = STATUS_STAY
        self.currentDir = vmath.vec3(0, 0, 1)
        self.gorlDir = vmath.vec3(0, 0, 1)
        print("Character __init__!")

    def onStart(self):
        print("onStart")
        print(f'Owner: {self.owner}')

        self.currentPosition = self.owner.transform.position
        self.ori_x, self.ori_y = (0.0, 0.0)
        self.x, self.y = (0.0, 0.0)
        self.wasClick = False

        figureComp = self.owner.getComponent("FigureComponent")
        if figureComp is not None:
            print(f"Figure component: {figureComp}")
            self.figure = figureComp.figure
            if self.figure is not None:
                self.figure.connectAnimator(
                    core.ANIMETION_SLOT_A0, STATE_MOTION[self.currentState])

    def changeStatus(self, status):
        if status != self.currentState and status != self.nextState:
            self.nextState = status
            self.figure.connectAnimator(
                core.ANIMETION_SLOT_A1, STATE_MOTION[status])
            self.transitTime = 0.0

    def transitMotion(self):
        if self.currentState != self.nextState:
            if self.transitTime >= TRANSIT_TIME:
                self.currentState = self.nextState
                self.figure.connectAnimator(
                    core.ANIMETION_SLOT_A0, STATE_MOTION[self.currentState])
                self.figure.connectAnimator(core.ANIMETION_SLOT_A1)
                return

            self.transitTime += core.getElapsedTime()
            if self.transitTime > TRANSIT_TIME:
                self.transitTime = TRANSIT_TIME
            self.figure.setBlendingWeight(
                core.ANIMETION_PART_A, self.transitTime / TRANSIT_TIME)

    def onUpdate(self, dt):
        r = vmath.dot(self.currentDir, self.gorlDir)
        if r < 0.99:
            n = vmath.cross(self.currentDir, self.gorlDir)
            ROT = vmath.mat_rotationY((1.0-r)*n.y*0.5, 3)
            self.currentDir = vmath.normalize(ROT * self.currentDir)

        if self.currentState == STATUS_RUN:
            self.currentPosition += self.currentDir * 3.0 * core.getElapsedTime()
        elif self.currentState == STATUS_WALK:
            self.currentPosition += self.currentDir * 1.0 * core.getElapsedTime()

        self.figure.rotation = vmath.normalize(
            vmath.quat_rotation((0, 0, 1), self.currentDir))
        self.figure.position = self.currentPosition

        self.transitMotion()

        scene = SceneManager.getInstance().currentScene
        if scene is not None:
            # print(f"scene: {scene}")
            camera = scene.activeCamera

            if camera is not None:
                moveVector = vmath.vec3(0.0, 0.0, 0.0)

                if core.isFingerPressed(0):
                    self.wasClick = True
                    self.ori_x, self.ori_y = core.getFingerPosition(0)

                if self.wasClick and core.isFingerMoved(0):
                    self.x, self.y = core.getFingerPosition(0)

                if core.isFingerReleased(0):
                    self.wasClick = False
                    self.ori_x, self.ori_y = (0.0, 0.0)
                    self.x, self.y = (0.0, 0.0)

                moveVector = vmath.vec3(self.x - self.ori_x, 0.0, -(self.y - self.ori_y))

                if vmath.almostEqual(moveVector, vmath.vec3(0.0, 0.0, 0.0)):
                    keyboard = Keyboard.instance()
                    if keyboard.isPressed(KeyCode.KEY_W):
                        moveVector = vmath.vec3(0.0, 0.0, -10.0)
                    elif keyboard.isPressed(KeyCode.KEY_S):
                        moveVector = vmath.vec3(0.0, 0.0, 10.0)
                    elif keyboard.isPressed(KeyCode.KEY_A):
                        moveVector = vmath.vec3(-10.0, 0.0, 0.0)
                    elif keyboard.isPressed(KeyCode.KEY_D):
                        moveVector = vmath.vec3(10.0, 0.0, 0.0)

                viewMat = camera.viewInverseMatrix
                if moveVector != vmath.vec3(0.0, 0.0, 0.0) and viewMat is not None:
                    moveVector = vmath.vec3(viewMat * moveVector)
                    l = vmath.length(moveVector)
                    if l > 100.0:
                        self.gorlDir = vmath.normalize(moveVector)
                        self.changeStatus(STATUS_RUN)
                    elif l > 5.0:
                        self.gorlDir = vmath.normalize(moveVector)
                        self.changeStatus(STATUS_WALK)
                else:
                    self.changeStatus(STATUS_STAY)
