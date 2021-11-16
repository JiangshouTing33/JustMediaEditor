package just.android.justmediaeditor.param

enum class FrameRateOption(val desc: String, val fps: Int) {
    FRAME_RATE_24("24",24),
    FRAME_RATE_25("25",25),
    FRAME_RATE_30("30",30),
    FRAME_RATE_50("50",50),
    FRAME_RATE_60("60",60);

    companion object {
        private val nameMap: MutableMap<String, FrameRateOption> = HashMap()

        init {
            for (i in values()) {
                nameMap[i.desc] = i
            }
        }

        fun fromName(name: String): FrameRateOption? {
            return nameMap[name]
        }
    }
}