"""
    Hello.py
    Script to print hello
"""

def Awake():
    print('Python function Awake() called')

def Start():
    print('Python function Enable() called')

def Enable():
    print('Python function Start() called')

def Disable():
    print('Python function Disable() called')

def Update(dt):
    print(f'Python function Update(%f) called', dt)

def FixedUpdate(dt):
    print(f'Python function FixedUpdate(%f) called', dt)

def LateUpdate():
    print(f'Python function LateUpdate(%f) called', dt)

def Render():
    print('Python function Render() called')

def Destroy():
    print('Python function Destroy() called')
