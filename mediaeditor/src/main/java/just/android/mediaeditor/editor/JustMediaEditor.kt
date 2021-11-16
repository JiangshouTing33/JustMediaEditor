package just.android.mediaeditor.editor

import android.content.Context
import android.graphics.Bitmap
import just.android.mediaeditor.constant.EditorDefaultParam
import just.android.mediaeditor.param.SpeedOption
import just.android.mediaeditor.param.VideoEditorParam
import just.android.mediaeditor.util.JustMEditorCommonUtil
import java.nio.ByteBuffer

val Bitmap.ByteData: ByteArray
    get() {
        val buf = ByteBuffer.allocate(byteCount)
        copyPixelsToBuffer(buf)
        return buf.array()
    }

enum class MediaEditorMessageType(val value: Int) {
    TYPE_ERROR(-1),
    TYPE_DURATION(0),
    TYPE_CURRENT_TIME(1),
    TYPE_DONE(2);

    companion object {
        val map: MutableMap<Int, MediaEditorMessageType> = HashMap()

        init {
            for (i in values()) {
                map[i.value] = i
            }
        }

        fun fromInt(type: Int?): MediaEditorMessageType? {
            return map[type]
        }
    }
}

object JustMediaEditor {
    var editEventListener: IEditEventListener? = null
    private var editorHandler: Long? = null
    var dstPath: String? = null
    var totalDuration: Float? = null

    @JvmStatic
    fun registerEditEventListener(eventListener: IEditEventListener?) {
        editEventListener = eventListener
        totalDuration?.let {
            editEventListener?.onEditEvent(MediaEditorMessageType.TYPE_DURATION, it)
        }
    }

    @JvmStatic
    fun startEditor(context: Context, editorParam: VideoEditorParam, eventListener: IEditEventListener? = null) {
        editEventListener = eventListener

        if (!init(editorParam.videoUrl, editorParam.audioUrl, editorParam.dstUrl)) return

        setCoverImage(editorParam.coverImage)
        setMuxerParam(editorParam.width, editorParam.height, editorParam.bitRate)
        setImageMarkData(editorParam.markImageOption?.bitmap, editorParam.markImageOption?.positionX, editorParam.markImageOption?.positionY)
        setLutFilter(editorParam.lutFilterBitmap)
        changeSpeed(editorParam.speedOption, editorParam.speedWithAudio, editorParam.backgroundMusicUrl)
        editorParam.stickerOptionList?.forEach {
            addStickerDisplayOption(it.bitmap,
                (it.durationStart * 1000).toLong(),
                (it.durationEnd * 1000).toLong(), it.positionX, it.positionY)
        }
        editorParam.textOptionList?.forEach {
            addTextDisplayParam(
                it.text,
                it.fontPath,
                (it.durationStart * 1000).toLong(), (it.durationEnd * 1000).toLong(),
                it.colorR, it.colorG, it.colorB,
                it.positionX, it.positionY)
        }
        dstPath = editorParam.dstUrl
        start()
    }

    @JvmStatic
    fun stopEditor() {
        unInit()
    }

    private fun init(srcVideoUrl: String?, srcAudioUrl: String?, dstUrl: String?): Boolean {
        if (dstUrl.isNullOrEmpty() || (srcVideoUrl.isNullOrEmpty() && srcAudioUrl.isNullOrEmpty())) {
            return false
        }

        if (editorHandler == null) {
            editorHandler = JustEditorNativeBridge.native_JustMediaEditor_Init(srcVideoUrl, srcAudioUrl, dstUrl)
        }

        return true
    }

    private fun unInit() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_UnInit(it)
        }
        editorHandler = null
    }

    private fun start() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_Start(it)
        }
    }

    private fun pause() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_Pause(it)
        }
    }

    private fun stop() {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_Stop(it)
        }
    }

    private fun setCoverImage(coverBitmap: Bitmap?, dstWidth: Int = 0, dstHeight: Int = 0) {
        if (coverBitmap != null) {
            editorHandler?.let {
                if (dstWidth == 0 || dstHeight == 0 || (coverBitmap.width == dstWidth && coverBitmap.height == dstHeight)) {
                    JustEditorNativeBridge.native_JustMediaEditor_SetCoverImage(
                        it, coverBitmap.width, coverBitmap.height, coverBitmap.ByteData,
                        EditorDefaultParam.DEFAULT_COVER_IMAGE_DURATION)
                } else {
                    val newBitmap = JustMEditorCommonUtil.scaleImage(coverBitmap, dstWidth, dstHeight)
                    if (newBitmap != null) {
                        JustEditorNativeBridge.native_JustMediaEditor_SetCoverImage(
                            it, newBitmap.width, newBitmap.height, newBitmap.ByteData,
                            EditorDefaultParam.DEFAULT_COVER_IMAGE_DURATION)
                    }
                }
            }
        }
    }

    private fun setMuxerParam(dstWidth: Int?, dstHeight: Int?, dstBitrate: Long?) {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_SetMuxerParam(
                it,
                dstWidth ?: 0,
                dstHeight ?: 0,
                dstBitrate ?: 0)
        }
    }

    private fun setImageMarkData(markBitmap: Bitmap?, markPositionX: Float?, markPositionY: Float?) {
        if (markBitmap != null) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustMediaEditor_SetImageMarkData(
                    it,
                    markBitmap.width, markBitmap.height, markBitmap.ByteData,
                    markPositionX ?: 0F, markPositionY ?: 0F)
            }
        }
    }

    private fun addTextDisplayParam(textStr: String, fontPath: String,
                            startTime: Long, endTime: Long,
                            red: Float, green: Float, blue: Float,
                            positionX: Float?, positionY: Float?) {
        editorHandler?.let {
            JustEditorNativeBridge.native_JustMediaEditor_AddTextDisplayParam(
                it,
                textStr.toByteArray(Charsets.UTF_32), fontPath,
                startTime, endTime,
                red, green, blue,
                positionX ?: 0F, positionY ?: 0F)
        }
    }

    private fun addStickerDisplayOption(stickerBitmap: Bitmap, startTime: Long, endTime: Long, positionX: Float? = 0F, positionY: Float? = 0F) {
        if (startTime >= 0 && endTime >= 0) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustMediaEditor_AddStickerDisplayOption(
                    it,
                    stickerBitmap.width, stickerBitmap.height, stickerBitmap.ByteData,
                    startTime, endTime,
                    positionX ?: 0F, positionY ?: 0F)
            }
        }
    }

    private fun setLutFilter(lutBitmap: Bitmap?) {
        if (lutBitmap != null) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustMediaEditor_SetLutImageData(it, lutBitmap.width, lutBitmap.height, lutBitmap.ByteData)
            }
        }
    }

    private fun setBackgroundMusic(backgroundAudioUrl: String, speed: SpeedOption = SpeedOption.SPEED_1_1) {
        editorHandler?.let {
            val desc = when (speed) {
                SpeedOption.SPEED_1_1 -> {
                    EditorDefaultParam.AUDIO_MIX_FILTER_DESC
                }
                SpeedOption.SPEED_4_1 -> {
                    EditorDefaultParam.AUDIO_MIX_FILTER_WITH_4_ATEMPO_DESC
                }
                else -> {
                    String.format(EditorDefaultParam.AUDIO_MIX_FILTER_WITH_ATEMPO_DESC, speed.desc)
                }
            }

            JustEditorNativeBridge.native_JustMediaEditor_SetAudioFilterData(
                it,
                desc,
                EditorDefaultParam.AUDIO_INPUT_ONE_NAME,
                EditorDefaultParam.AUDIO_INPUT_TWO_NAME,
                EditorDefaultParam.AUDIO_OUTPUT_NAME)
            JustEditorNativeBridge.native_JustMediaEditor_SetBackgroundMusicData(it, backgroundAudioUrl)
        }
    }

    private fun changeAudioSpeed(speed: SpeedOption) {
        editorHandler?.let {
            val desc = when (speed) {
                SpeedOption.SPEED_1_1 -> {
                    return
                }
                SpeedOption.SPEED_4_1 -> {
                    EditorDefaultParam.AUDIO_4_TEMPO_FILTER_DESC
                }
                else -> {
                    String.format(EditorDefaultParam.AUDIO_TEMPO_FILTER_DESC, speed.desc)
                }
            }
            JustEditorNativeBridge.native_JustMediaEditor_SetAudioFilterData(
                it,
                desc,
                EditorDefaultParam.AUDIO_INPUT_ONE_NAME,
                null,
                EditorDefaultParam.AUDIO_OUTPUT_NAME)
        }
    }

    private fun changeSpeed(speed: SpeedOption?, withAudio: Boolean = true, backgroundAudioUrl: String? = null) {
        if (speed != null) {
            editorHandler?.let {
                JustEditorNativeBridge.native_JustMediaEditor_ChangeVideoSpeed(it, speed.value)

                if (withAudio) {
                    if (!backgroundAudioUrl.isNullOrEmpty()) {
                        setBackgroundMusic(backgroundAudioUrl, speed)
                    } else {
                        changeAudioSpeed(speed)
                    }
                }
            }
        }
    }
}