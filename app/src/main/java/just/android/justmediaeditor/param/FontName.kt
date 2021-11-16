package just.android.justmediaeditor.param

enum class FontName(val desc: String, val path: String) {
    FONT_ANTONIO("Antonio", "Antonio-Regular.ttf"),
    FONT_JIANTI("华文仿宋", "jianti.ttf"),
    FONT_MSYH("MSYH", "msyh.ttc"),
    FONT_TIMES_BI("TimesBi", "timesbi.ttf");

    companion object {
        private val nameMap: MutableMap<String, FontName> = HashMap()

        init {
            for (i in values()) {
                nameMap[i.desc] = i
            }
        }

        fun fromName(name: String): FontName? {
            return nameMap[name]
        }
    }
}