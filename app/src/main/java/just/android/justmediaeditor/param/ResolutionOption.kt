package just.android.justmediaeditor.param

enum class ResolutionOption(val desc: String, val width: Int, val height: Int, val bitRate: Long) {
    RESOLUTION_ORIGIN("原画", 0, 0, 0),
    RESOLUTION_480_P("480p", 720, 480, 2000000),
    RESOLUTION_720_P("720p", 1280, 720, 3000000),
    RESOLUTION_1080_P("1080p", 1920, 1080, 5000000);

    companion object {
        private val nameMap: MutableMap<String, ResolutionOption> = HashMap()

        init {
            for (i in values()) {
                nameMap[i.desc] = i
            }
        }

        fun fromName(name: String): ResolutionOption? {
            return nameMap[name]
        }
    }
}