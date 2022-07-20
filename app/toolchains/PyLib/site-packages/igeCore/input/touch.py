"""
indi game engine - touch input
"""
import igeCore as core
import igeVmath as vmath

try:
    from igeScene import SceneManager
except:
    print("igeScene not installed")

class Touch:
    '''
    Class Touch
    '''

    @staticmethod
    def getPosition(fingerIndex):
        """
        Get a finger position by the index of the finger.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            [x (int), y (int)] : Position
        """
        x, y = core.getFingerPosition(fingerIndex)
        if SceneManager is not None:
            scene = SceneManager.getInstance().currentScene
            if scene is not None:
                delta = (scene.getViewSize() - scene.getScreenSize()) * 0.5
                x = x - delta.x
                y = y + delta.y
        return [x, y]

    @staticmethod
    def isPressed(fingerIndex):
        """
        Check if a finger has pressed.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            True: If the finger has pressed
            False: If the finger has not pressed
        """
        return core.isFingerPressed(fingerIndex)

    @staticmethod
    def isMoved(fingerIndex):
        """
        Check if a finger has moved.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            True: If the finger has moved
            False: If the finger has not moved
        """
        return core.isFingerMoved(fingerIndex)

    @staticmethod
    def isReleased(fingerIndex):
        """
        Check if a finger has released.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            True: If the finger has released
            False: If the finger has not released
        """
        return core.isFingerReleased(fingerIndex)

    @staticmethod
    def isScrolled(fingerIndex):
        """
        Check if a mouse has scrolled.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            True: If the mouse has scrolled
            False: If the mouse has not scrolled
        """
        return core.isFingerScrolled(fingerIndex)

    @staticmethod
    def getScrollData(fingerIndex):
        """
        Get scrolling data if mouse has scrolled.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            scrollValue (float)
        """
        return core.getFingerScrolledData(fingerIndex)

    @staticmethod
    def getPressure(fingerIndex):
        """
        Get the pressure value from a finger.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            pressureValue (int)
        """
        return core.getFingerPressure(fingerIndex)

    @staticmethod
    def getId(fingerIndex):
        """
        Get the finger unique id from finger index.
        This is useful for multiple touch application when user need to check the source of an event.

        Parameters
        ----------
            fingerIndex (int): The finger index to check

        Returns
        -------
            fingerId (int)
        """
        return core.getFingerId(fingerIndex)

    @staticmethod
    def count():
        """
        Get the total number of active fingers on screen.

        Returns
        -------
            numberOfFingers (int)
        """
        return core.getFingersCount()
