package org.opencv

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.*
import android.hardware.camera2.*
import android.media.ImageReader
import android.os.Bundle
import android.os.Handler
import android.os.HandlerThread
import androidx.annotation.NonNull
import android.util.Log
import android.util.Size
import android.view.Surface
import android.widget.Toast
import java.lang.Long.signum
import java.util.*
import java.util.concurrent.Semaphore
import java.util.concurrent.TimeUnit
import android.hardware.camera2.CameraCharacteristics
import android.graphics.ImageFormat
import androidx.core.app.ActivityCompat
import org.libsdl.app.SDLActivity


open class CameraActivity: SDLActivity() {
    private lateinit var cameraId: String
    private var cameraDevice: CameraDevice? = null
    private var captureSession: CameraCaptureSession? = null
    private val cameraOpenCloseLock = Semaphore(1)

    private lateinit var imageDimension: Size
    private var mBackgroundHandler: Handler? = null
    private var mBackgroundThread: HandlerThread? = null
    private var mImageReader: ImageReader? = null
    private var maxFastYuvRate: Long = 33333333 // @30 FPS
    private var m_camIdx: Int = 0
    private var m_camWidth = 640
    private var m_camHeight = 480

    override fun onCreate(savedInstanceState: Bundle?) {
        Log.e(TAG, "onCreate")
        super.onCreate(savedInstanceState)
        sInstance = this
    }

    override fun onResume() {
        Log.e(TAG, "onResume")
        super.onResume()
        startBackgroundThread()
    }

    override fun onPause() {
        Log.e(TAG, "onPause")
        super.onPause()
        stopBackgroundThread()
    }

    override fun onDestroy() {
        Log.e(TAG, "onDestroy")
        closeCamera()
        super.onDestroy()
        sInstance = null
    }

    private val stateCallback = object : CameraDevice.StateCallback() {
        override fun onOpened(camera: CameraDevice) {
            Log.e(TAG, "onOpened")
            cameraOpenCloseLock.release()
            cameraDevice = camera
            createCameraPreview()
        }

        override fun onDisconnected(camera: CameraDevice) {
            cameraOpenCloseLock.release()
            cameraDevice!!.close()
        }

        override fun onError(camera: CameraDevice, error: Int) {
            cameraOpenCloseLock.release()
            cameraDevice!!.close()
            cameraDevice = null
        }
    }

    private val captureCallback = object : CameraCaptureSession.CaptureCallback() {
        override fun onCaptureProgressed(session: CameraCaptureSession,
                                         request: CaptureRequest,
                                         partialResult: CaptureResult) {
            // DO NOTHING
        }

        override fun onCaptureCompleted(session: CameraCaptureSession,
                                        request: CaptureRequest,
                                        result: TotalCaptureResult) {
            // DO NOTHING
        }
    }

    private fun startBackgroundThread() {
        mBackgroundThread = HandlerThread("Camera Background")
        mBackgroundThread!!.start()
        mBackgroundHandler = Handler(mBackgroundThread!!.looper)
    }

    private fun stopBackgroundThread() {
        mBackgroundThread!!.quitSafely()

        try {
            mBackgroundThread!!.join()
            mBackgroundThread = null
            mBackgroundHandler = null
        } catch (e: InterruptedException) {
            e.printStackTrace()
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        @NonNull
        permissions: Array<String>, @NonNull
        grantResults: IntArray
    ) {
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults[0] == PackageManager.PERMISSION_DENIED) {
                // close the app
                Toast.makeText(
                    this@CameraActivity,
                    "Sorry!!!, you can't use this app without granting permission",
                    Toast.LENGTH_LONG
                ).show()
                finish()
            }
            else if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                // Reopen the camera
                openCamera(m_camIdx, m_camWidth, m_camHeight)
            }
        }
    }

    private fun openCamera(idx: Int, width: Int, height: Int) {
        m_camIdx = idx
        m_camWidth = width
        m_camHeight = height

        // Add permission for camera and let user grant the permission
        if (ActivityCompat.checkSelfPermission(
                this,
                Manifest.permission.CAMERA
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                this@CameraActivity,
                arrayOf(Manifest.permission.CAMERA),
                REQUEST_CAMERA_PERMISSION
            )
            return
        }

        setUpCameraOutputs(idx, width, height)

        val manager = getSystemService(Context.CAMERA_SERVICE) as CameraManager
        try {
            // Wait for camera to open - 2.5 seconds is sufficient
            if (!cameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw RuntimeException("Time out waiting to lock camera opening.")
            }
            runOnUiThread(Runnable { manager.openCamera(cameraId, stateCallback, mBackgroundHandler) })
        } catch (e: CameraAccessException) {
            Log.e(TAG, e.toString())
        } catch (e: InterruptedException) {
            throw RuntimeException("Interrupted while trying to lock camera opening.", e)
        }
    }

    private fun closeCamera() {
        try {
            cameraOpenCloseLock.acquire()
            captureSession?.close()
            captureSession = null
            cameraDevice?.close()
            cameraDevice = null
            mImageReader?.close()
            mImageReader = null
        } catch (e: Exception) {
        } finally {
            cameraOpenCloseLock.release()
        }
    }

    protected fun createCameraPreview() {
        try {
            // We set up a CaptureRequest.Builder with the output Surface.
            val previewRequestBuilder = cameraDevice!!.createCaptureRequest(
                CameraDevice.TEMPLATE_PREVIEW
            )
            var surface =  mImageReader?.surface
            previewRequestBuilder.addTarget(surface!!)

            // Here, we create a CameraCaptureSession for camera preview.
            cameraDevice?.createCaptureSession(Arrays.asList(surface),
                object : CameraCaptureSession.StateCallback() {

                    override fun onConfigured(cameraCaptureSession: CameraCaptureSession) {
                        // The camera is already closed
                        if (cameraDevice == null) return

                        // When the session is ready, we start displaying the preview.
                        captureSession = cameraCaptureSession
                        try {
                            // Auto mode
                            previewRequestBuilder.set(CaptureRequest.CONTROL_MODE, CaptureRequest.CONTROL_MODE_USE_SCENE_MODE)
                            previewRequestBuilder.set(CaptureRequest.CONTROL_SCENE_MODE, CaptureRequest.CONTROL_SCENE_MODE_ACTION)

                            // Try acquire max supported FPS
                            previewRequestBuilder.set(CaptureRequest.SENSOR_FRAME_DURATION, maxFastYuvRate)

                            // Finally, we start displaying the camera preview.
                            val previewRequest = previewRequestBuilder.build()
                            captureSession?.setRepeatingRequest(previewRequest,
                                captureCallback, mBackgroundHandler)
                        } catch (e: CameraAccessException) {
                            Log.e(TAG, e.toString())
                        }
                    }

                    override fun onConfigureFailed(session: CameraCaptureSession) {
                        Log.e(TAG, "Camera failed to configure")
                    }
                }, null)
        } catch (e: CameraAccessException) {
            Log.e(TAG, e.toString())
        }
    }

    private fun areDimensionsSwapped(displayRotation: Int, sensorOrientation: Int): Boolean {
        var swappedDimensions = false
        when (displayRotation) {
            Surface.ROTATION_0, Surface.ROTATION_180 -> {
                if (sensorOrientation == 90 || sensorOrientation == 270) {
                    swappedDimensions = true
                }
            }
            Surface.ROTATION_90, Surface.ROTATION_270 -> {
                if (sensorOrientation == 0 || sensorOrientation == 180) {
                    swappedDimensions = true
                }
            }
            else -> {
                Log.e(TAG, "Display rotation is invalid: $displayRotation")
            }
        }
        return swappedDimensions
    }

    /**
     * Sets up member variables related to camera.
     *
     * @param width  The width of available size for camera preview
     * @param height The height of available size for camera preview
     */
    private fun setUpCameraOutputs(idx:Int, width: Int, height: Int) {
        val manager = getSystemService(Context.CAMERA_SERVICE) as CameraManager
        try {
            cameraId = manager.cameraIdList[idx]
            val characteristics = manager.getCameraCharacteristics(cameraId)
            val map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)
            val dims = map!!.getOutputSizes(SurfaceTexture::class.java)

            val sizes = map.getOutputSizes(ImageFormat.YUV_420_888)
            for(size in sizes) {
                Log.d(TAG, "Supported: $size")
            }

            imageDimension = Size(640, 480)
            for (dim in dims) {
                if(dim.width == width && dim.height == height) {
                    imageDimension = Size(width, height)
                    break
                }
            }

            maxFastYuvRate = map.getOutputMinFrameDuration(
                ImageFormat.YUV_420_888,
                imageDimension
            )

            val displayRotation = windowManager.defaultDisplay.rotation
            val sensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION)
            val swappedDimensions = areDimensionsSwapped(displayRotation, sensorOrientation!!)

            val displaySize = Point()
            windowManager.defaultDisplay.getSize(displaySize)

            val rotatedPreviewWidth = if (swappedDimensions) imageDimension.height else imageDimension.width
            val rotatedPreviewHeight = if (swappedDimensions) imageDimension.width else imageDimension.height

            mImageReader = ImageReader.newInstance(
                imageDimension.width, imageDimension.height,
                ImageFormat.YUV_420_888, 4
            )

            this.cameraId = cameraId
        } catch (e: CameraAccessException) {
            Log.e(TAG, e.toString())
        } catch (e: NullPointerException) {
            Log.e(TAG, e.toString())
        }
    }

    internal class CompareSizesByArea : Comparator<Size> {
        // We cast here to ensure the multiplications won't overflow
        override fun compare(lhs: Size, rhs: Size) =
            signum(lhs.width.toLong() * lhs.height - rhs.width.toLong() * rhs.height)
    }

    companion object {
        private val REQUEST_CAMERA_PERMISSION = 200

        @JvmStatic
        var sInstance:CameraActivity? = null

        @JvmStatic
        private val TAG = "AndroidCameraApi"

        @JvmStatic private fun chooseOptimalSize(
            choices: Array<Size>,
            textureViewWidth: Int,
            textureViewHeight: Int,
            maxWidth: Int,
            maxHeight: Int,
            aspectRatio: Size
        ): Size {
            // Collect the supported resolutions that are at least as big as the preview Surface
            val bigEnough = ArrayList<Size>()
            // Collect the supported resolutions that are smaller than the preview Surface
            val notBigEnough = ArrayList<Size>()
            val w = aspectRatio.width
            val h = aspectRatio.height
            for (option in choices) {
                if (option.width <= maxWidth && option.height <= maxHeight &&
                    option.height == option.width * h / w) {
                    if (option.width >= textureViewWidth && option.height >= textureViewHeight) {
                        bigEnough.add(option)
                    } else {
                        notBigEnough.add(option)
                    }
                }
            }

            // Pick the smallest of those big enough. If there is no one big enough, pick the
            // largest of those not big enough.
            if (bigEnough.size > 0) {
                return Collections.min(bigEnough, CompareSizesByArea())
            } else if (notBigEnough.size > 0) {
                return Collections.max(notBigEnough, CompareSizesByArea())
            } else {
                Log.e(TAG, "Couldn't find any suitable preview size")
                return choices[0]
            }
        }
    }
}
