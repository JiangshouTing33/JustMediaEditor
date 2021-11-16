package just.android.mediaeditor.param

enum class SpeedOption(val desc: String, val value: Float) {
    SPEED_1_2("0.5", 0.5F),
    SPEED_1_1("1", 1F),
    SPEED_2_3("1.5", 1.5F),
    SPEED_2_1("2", 2F),
    SPEED_4_1("4", 4F);

    companion object {
        private val map: MutableMap<Float, SpeedOption> = HashMap()
        private val nameMap: MutableMap<String, SpeedOption> = HashMap()

        init {
            for (i in values()) {
                map[i.value] = i
                nameMap[i.desc] = i
            }
        }

        fun fromInt(type: Float?): SpeedOption? {
            return map[type]
        }

        fun fromName(name: String): SpeedOption? {
            return nameMap[name]
        }
    }
}