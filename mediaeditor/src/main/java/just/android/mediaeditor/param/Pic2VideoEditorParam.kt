package just.android.mediaeditor.param

import android.graphics.Bitmap

class Pic2VideoEditorParam private constructor(
    var multiPicPath: List<String>?,
    var transitionTypeGlsl: String?,
    var fps: Int?,
    width: Int?,
    height: Int?,
    bitRate: Long?,
    audioUrl: String?,
    dstUrl: String,
    coverImage: Bitmap?,
    markImageOption: MarkImageOption?,
    stickerOptionList: List<StickerImageOption>?,
    textOptionList: List<FontTextOption>?):
    BaseEditorParam(width, height, bitRate, audioUrl, dstUrl, coverImage, markImageOption, stickerOptionList, textOptionList){

    private constructor(builder: Builder): this (
        builder.multiPicPath,
        builder.transitionTypeGlsl,
        builder.fps,
        builder.width,
        builder.height,
        builder.bitRate,
        builder.audioUrl,
        builder.dstUrl,
        builder.coverImage,
        builder.markImageOption,
        builder.stickerOptionList,
        builder.textOptionList
    )

    class Builder {
        var multiPicPath: List<String>? = null
            private set
        var transitionTypeGlsl: String? = null
            private set
        var fps: Int? = null
            private set
        var width: Int? = null
            private set
        var height: Int? = null
            private set
        var bitRate: Long? = null
            private set
        var audioUrl: String? = null
            private set
        lateinit var dstUrl: String
            private set
        var coverImage: Bitmap? = null
            private set
        var markImageOption: MarkImageOption? = null
            private set
        var stickerOptionList: List<StickerImageOption>? = null
            private set
        var textOptionList: List<FontTextOption>? = null
            private set

        fun setMultiPicPath(multiPicPath: List<String>?) = apply {
            this.multiPicPath = multiPicPath
        }

        fun setTransitionTypeGlsl(transitionTypeGlsl: String?) = apply {
            this.transitionTypeGlsl = transitionTypeGlsl
        }

        fun setFps(fps: Int?) = apply {
            this.fps = fps
        }

        fun setWidth(width: Int?) = apply {
            this.width = width
        }

        fun setHeight(height: Int?) = apply {
            this.height = height
        }

        fun setBitrate(bitRate: Long?) = apply {
            this.bitRate = bitRate
        }

        fun setAudioUrl(audioUrl: String?) = apply {
            this.audioUrl = audioUrl
        }

        fun setDstUrl(dstUrl: String) = apply {
            this.dstUrl = dstUrl
        }

        fun setCoverImage(coverImage: Bitmap?) = apply {
            this.coverImage = coverImage
        }

        fun setMarkImageOption(markImageOption: MarkImageOption?) = apply {
            this.markImageOption = markImageOption
        }

        fun setStickerImageOptionList(stickerOptionList: List<StickerImageOption>?) = apply {
            this.stickerOptionList = stickerOptionList
        }

        fun setFontTextOptionList(textOptionList: List<FontTextOption>?) = apply {
            this.textOptionList = textOptionList
        }

        fun build() = Pic2VideoEditorParam(this)
    }
}