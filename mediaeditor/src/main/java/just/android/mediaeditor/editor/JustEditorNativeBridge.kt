package just.android.mediaeditor.editor

object JustEditorNativeBridge {
    init {
        try {
            System.loadLibrary("just-meditor")
        } catch (e: UnsatisfiedLinkError) {
            e.printStackTrace()
        } catch (e: SecurityException) {
            e.printStackTrace()
        }
    }

    private fun editorMessageCallback(messageType: Int, messageValue: Float) {
        if (messageType == MediaEditorMessageType.TYPE_DURATION.value) {
            JustMediaEditor.totalDuration = messageValue
        } else if (messageType == MediaEditorMessageType.TYPE_DONE.value
            || messageType == MediaEditorMessageType.TYPE_ERROR.value) {
            JustMediaEditor.totalDuration = null
        }
        JustMediaEditor.editEventListener?.onEditEvent(MediaEditorMessageType.fromInt(messageType), messageValue)
    }

    private fun pic2VideoEditorMessageCallback(messageType: Int, messageValue: Float) {
        if (messageType == MediaEditorMessageType.TYPE_DURATION.value) {
            JustPic2VideoEditor.totalDuration = messageValue
        } else if (messageType == MediaEditorMessageType.TYPE_DONE.value
            || messageType == MediaEditorMessageType.TYPE_ERROR.value) {
            JustPic2VideoEditor.totalDuration = null
        }
        JustPic2VideoEditor.pic2VideoEditEventListener?.onEditEvent(
            MediaEditorMessageType.fromInt(
                messageType
            ), messageValue)
    }

    //JustMediaEditor
    external fun native_JustMediaEditor_Init(srcVideoUrl: String?, srcAudioUrl: String?, dstUrl: String?): Long
    external fun native_JustMediaEditor_UnInit(editorHandler: Long)
    external fun native_JustMediaEditor_Start(editorHandler: Long)
    external fun native_JustMediaEditor_Pause(editorHandler: Long)
    external fun native_JustMediaEditor_Stop(editorHandler: Long)
    external fun native_JustMediaEditor_SetCoverImage(editorHandler: Long, width: Int, height: Int, data: ByteArray, duration: Double)
    external fun native_JustMediaEditor_SetImageMarkData(editorHandler: Long, width: Int, height: Int,
                                                         data: ByteArray, markPositionX: Float, markPositionY: Float)
    external fun native_JustMediaEditor_AddTextDisplayParam(editorHandler: Long, textStr: ByteArray, fontName: String,
                                                            startTime: Long, endTime: Long,
                                                            red: Float, green: Float, blue: Float,
                                                            positionX: Float, positionY: Float)
    external fun native_JustMediaEditor_AddStickerDisplayOption(editorHandler: Long,
                                                                width: Int, height: Int, data: ByteArray,
                                                                startTime: Long, endTime: Long,
                                                                positionX: Float, positionY: Float)
    external fun native_JustMediaEditor_SetLutImageData(editorHandler: Long, width: Int, height: Int,
                                                        data: ByteArray)
    external fun native_JustMediaEditor_SetAudioFilterData(editorHandler: Long, filterDesc: String,
                                                           inputNameOne: String, inputNameTwo: String?, outputName: String)
    external fun native_JustMediaEditor_SetBackgroundMusicData(editorHandler: Long, backgroundAudioUrl: String)
    external fun native_JustMediaEditor_ChangeVideoSpeed(editorHandler: Long, videoSpeed: Float)
    external fun native_JustMediaEditor_SetMuxerParam(editorHandler: Long, dstWidth: Int, dstHeight: Int, dstBitrate: Long)

    //JustPic2VideoEditor
    external fun native_JustPic2VideoEditor_Init(srcAudioUrl: String, dstUrl: String, dstWidth: Int, dstHeight: Int, fps: Int, videoBitrate: Long): Long
    external fun native_JustPic2VideoEditor_UnInit(editorHandler: Long)
    external fun native_JustPic2VideoEditor_Start(editorHandler: Long)
    external fun native_JustPic2VideoEditor_Pause(editorHandler: Long)
    external fun native_JustPic2VideoEditor_Stop(editorHandler: Long)
    external fun native_JustPic2VideoEditor_SetCoverImage(editorHandler: Long, width: Int, height: Int, data: ByteArray, duration: Double)
    external fun native_JustPic2VideoEditor_SetImageMarkData(editorHandler: Long, width: Int, height: Int,
                                                             data: ByteArray, markPositionX: Float, markPositionY: Float)
    external fun native_JustPic2VideoEditor_AddTextDisplayParam(editorHandler: Long, textStr: ByteArray, fontName: String,
                                                                startTime: Long, endTime: Long,
                                                                red: Float, green: Float, blue: Float,
                                                                positionX: Float, positionY: Float)
    external fun native_JustPic2VideoEditor_AddStickerDisplayOption(editorHandler: Long,
                                                                    width: Int, height: Int, data: ByteArray,
                                                                    startTime: Long, endTime: Long,
                                                                    positionX: Float, positionY: Float)
    external fun native_JustPic2VideoEditor_SetImageSourceWithIndex(editorHandler: Long, index: Int,
                                                                    width: Int, height: Int, data: ByteArray)
    external fun native_JustPic2VideoEditor_SetImageCount(editorHandler: Long, imageCount: Int)
    external fun native_JustPic2VideoEditor_SetTransitionFShaderStr(editorHandler: Long, fShaderStr: String)
}