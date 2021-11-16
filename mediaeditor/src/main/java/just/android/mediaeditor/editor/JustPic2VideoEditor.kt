package just.android.mediaeditor.editor

import android.content.Context
import android.graphics.Bitmap
import just.android.mediaeditor.constant.EditorDefaultParam
import just.android.mediaeditor.param.Pic2VideoEditorParam
import just.android.mediaeditor.util.JustMEditorCommonUtil

object JustPic2VideoEditor {
    private const val DEFAULT_FPS = 30
    private const val DEFAULT_VIDEO_BITRATE = 5000 * 1000L

    private var editorHandler: Long? = null
    var pic2VideoEditEventListener: IEditEventListener? = null
    var dstPath: String? = null
    var totalDuration: Float? = null

    @JvmStatic
    fun registerEditorEventListener(eventListener: IEditEventListener?) {
        pic2VideoEditEventListener = eventListener
        totalDuration?.let {
            pic2VideoEditEventListener?.onEditEvent(MediaEditorMessageType.TYPE_DURATION, it)
        }
    }

    @JvmStatic
    fun startEditor(context: Context, editorParam: Pic2VideoEditorParam, editEventListener: IEditEventListener? = null) {
        pic2VideoEditEventListener = editEventListener
        if (!init(editorParam.audioUrl, editorParam.dstUrl, editorParam.width, editorParam.height, fps = editorParam.fps, editorParam.bitRate)) return

        setCoverImage(editorParam.coverImage, editorParam.width, editorParam.height)

        setImageMarkData(
            editorParam.markImageOption?.bitmap,
            editorParam.markImageOption?.positionX,
            editorParam.markImageOption?.positionY
        )

        editorParam.stickerOptionList?.forEach {
            addStickerDisplayOption(
                it.bitmap,
                (it.durationStart * 1000).toLong(),
                (it.durationEnd * 1000).toLong(), it.positionX, it.positionY
            )
        }

        editorParam.textOptionList?.forEach {
            addTextDisplayParam(
                it.text,
                it.fontPath,
                (it.durationStart * 1000).toLong(), (it.durationEnd * 1000).toLong(),
                it.colorR, it.colorG, it.colorB,
                it.positionX, it.positionY
            )
        }
        editorParam.multiPicPath?.let {
            for ((index, value) in it.withIndex()) {
                setImageSourceWithIndex(context, value, index)
            }
            setImageCount(it.size)
        }

        setTransitionShaderStr(editorParam.transitionTypeGlsl)
        dstPath = editorParam.dstUrl
        start()
    }

    @JvmStatic
    fun stopEditor() {
        unInit()
    }

    private fun init(srcAudioUrl: String?, dstUrl: String, dstWidth: Int?, dstHeight: Int?,
                     fps: Int? = DEFAULT_FPS, videoBitrate: Long? = DEFAULT_VIDEO_BITRATE): Boolean {
        if (srcAudioUrl.isNullOrEmpty() || dstUrl.isEmpty() || dstWidth == null || dstWidth <= 0 || dstHeight == null || dstHeight <= 0) {
            return false
        }

        val fpsValue = if (fps == null || fps < 0) DEFAULT_FPS else fps
        val videoBitrateValue = if (videoBitrate == null || videoBitrate < 0) DEFAULT_VIDEO_BITRATE else videoBitrate

        editorHandler = JustEditorNativeBridge.native_JustPic2VideoEditor_Init(srcAudioUrl, dstUrl, dstWidth, dstHeight, fpsValue, videoBitrateValue)
        return true
    }

    private fun unInit() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_UnInit(it)
        }
    }

    fun start() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_Start(it)
        }
    }

    fun pause() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_Pause(it)
        }
    }

    fun stop() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_Stop(it)
        }
    }

    private fun setCoverImage(coverBitmap: Bitmap?, dstWidth: Int? = 0, dstHeight: Int? = 0) {
        if (coverBitmap != null) {
            editorHandler?.let {
                if (dstWidth != null && dstHeight != null && (coverBitmap.width != dstWidth || coverBitmap.height != dstHeight)) {
                    val newBitmap = JustMEditorCommonUtil.scaleImage(coverBitmap, dstWidth, dstHeight)
                    if (newBitmap != null) {
                        JustEditorNativeBridge.native_JustPic2VideoEditor_SetCoverImage(it, newBitmap.width, newBitmap.height, newBitmap.ByteData,
                            EditorDefaultParam.DEFAULT_COVER_IMAGE_DURATION
                        )
                    }
                } else {
                    JustEditorNativeBridge.native_JustPic2VideoEditor_SetCoverImage(it, coverBitmap.width, coverBitmap.height, coverBitmap.ByteData,
                        EditorDefaultParam.DEFAULT_COVER_IMAGE_DURATION
                    )
                }
            }
        }
    }

    private fun setImageMarkData(markBitmap: Bitmap?, markPositionX: Float?, markPositionY: Float?) {
        if (markBitmap != null) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustPic2VideoEditor_SetImageMarkData(
                    it,
                    markBitmap.width, markBitmap.height, markBitmap.ByteData,
                    markPositionX ?: 0F, markPositionY ?: 0F)
            }
        }
    }

    private fun addTextDisplayParam(textStr: String, fontName: String,
                            startTime: Long, endTime: Long,
                            red: Float, green: Float, blue: Float,
                            positionX: Float, positionY: Float) {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_AddTextDisplayParam(
                it,
                textStr.toByteArray(Charsets.UTF_32), fontName,
                startTime, endTime,
                red, green, blue,
                positionX, positionY)
        }
    }

    private fun addStickerDisplayOption(stickerBitmap: Bitmap, startTime: Long, endTime: Long, positionX: Float? = 0F, positionY: Float? = 0F) {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_AddStickerDisplayOption(
                it,
                stickerBitmap.width, stickerBitmap.height, stickerBitmap.ByteData,
                startTime, endTime,
                positionX ?: 0F, positionY ?: 0F)
        }
    }

    private fun setImageSourceWithIndex(context: Context, path: String, index: Int) {
        val bitmap = JustMEditorCommonUtil.generateBitmapFromUriString(context, path)
        if (bitmap != null) {
            editorHandler?.let {
                val bitmap = JustMEditorCommonUtil.scaleImage(bitmap, 1920, 1080)
                if (bitmap != null) {
                    JustEditorNativeBridge.native_JustPic2VideoEditor_SetImageSourceWithIndex(it, index, bitmap.width, bitmap.height, bitmap.ByteData)
                }
            }
        }
    }

    private fun setImageCount(imageCount: Int) {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustPic2VideoEditor_SetImageCount(it, imageCount)
        }
    }

    private fun setTransitionShaderStr(transitionGlsl: String?) {
        if (transitionGlsl != null) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustPic2VideoEditor_SetTransitionFShaderStr(it, transitionGlsl)
            }
        }
    }
}