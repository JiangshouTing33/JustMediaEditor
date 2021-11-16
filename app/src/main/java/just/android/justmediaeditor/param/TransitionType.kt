package just.android.justmediaeditor.param

enum class TransitionType(val value: Int, val fileName: String, val transitionName: String) {
    HUANYING(0, "glTransitionFShader_huanying.glsl", "幻影"),
    SHUIZI(1, "glTransitionFShader_shuizi.glsl", "水渍"),
    XIANTIAO(2, "glTransitionFShader_xiantiao.glsl", "线条"),
    CHONGDIE(3, "glTransitionFShader_chongdie.glsl", "重叠"),
    SHUIDI(4, "glTransitionFShader_shuidi.glsl", "水滴"),
    SHUNSHIZHEN(5, "glTransitionFShader_shunshizhen.glsl", "顺时针");

    companion object {
        private val map: MutableMap<Int, TransitionType> = HashMap()
        private val nameMap: MutableMap<String, TransitionType> = HashMap()

        init {
            for (i in values()) {
                map[i.value] = i
                nameMap[i.transitionName] = i
            }
        }

        fun fromInt(type: Int?): TransitionType? {
            return map[type]
        }

        fun fromName(name: String): TransitionType? {
            return nameMap[name]
        }
    }
}