package just.android.mediaeditor.param

import android.graphics.Bitmap

class VideoEditorParam private constructor(
    var videoUrl: String?,
    var backgroundMusicUrl: String?,
    var lutFilterBitmap: Bitmap?,
    var speedOption: SpeedOption?,
    var speedWithAudio: Boolean,
    width: Int?,
    height: Int?,
    bitRate: Long?,
    audioUrl: String?,
    dstUrl: String,
    coverImage: Bitmap?,
    markImageOption: MarkImageOption?,
    stickerOptionList: List<StickerImageOption>?,
    textOptionList: List<FontTextOption>?):
    BaseEditorParam(width, height, bitRate, audioUrl, dstUrl, coverImage, markImageOption, stickerOptionList, textOptionList) {

    private constructor(builder: Builder): this (
        builder.videoUrl,
        builder.backgroundMusicUrl,
        builder.lutFilterBitmap,
        builder.speedOption,
        builder.speedWithAudio,
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
        var videoUrl: String? = null
            private set
        var backgroundMusicUrl: String? = null
            private set
        var lutFilterBitmap: Bitmap? = null
            private set
        var speedOption: SpeedOption? = null
            private set
        var speedWithAudio: Boolean = true
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

        fun setWidth(width: Int?) = apply {
            this.width = width
        }

        fun setHeight(height: Int?) = apply {
            this.height = height
        }

        fun setBitrate(bitRate: Long?) = apply {
            this.bitRate = bitRate
        }

        fun setVideoUrl(videoUrl: String?) = apply {
            this.videoUrl = videoUrl
        }

        fun setAudioUrl(audioUrl: String?) = apply {
            this.audioUrl = audioUrl
        }

        fun setBackgroundMusicUrl(backgroundMusicUrl: String?) = apply {
            this.backgroundMusicUrl = backgroundMusicUrl
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

        fun setLutFilterBitmap(lutFilterBitmap: Bitmap?) = apply {
            this.lutFilterBitmap = lutFilterBitmap
        }

        fun setSpeedOption(speedOption: SpeedOption?) = apply {
            this.speedOption = speedOption
        }

        fun setSpeedWithAudio(speedWithAudio: Boolean) = apply {
            this.speedWithAudio = speedWithAudio
        }

        fun build() = VideoEditorParam(this)
    }
}