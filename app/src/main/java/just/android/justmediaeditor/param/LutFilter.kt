package just.android.justmediaeditor.param

import androidx.annotation.DrawableRes
import just.android.justmediaeditor.R

enum class LutFilter(@DrawableRes val lutDrawableId: Int, val lutName: String, val lutTag: String, val lutValue: Int) {
    AIQING(R.drawable.beauty_filter_aiqing, "爱情", "AIQING", 0),
    BAINEN(R.drawable.beauty_filter_bainen, "白嫩",  "BAINEN", 1),
    BAIXI(R.drawable.beauty_filter_baixi, "白皙",  "BAIXI",2),
    CHUNYI(R.drawable.beauty_filter_chunyi, "春意",  "CHUNYI",3),
    CHUWEN(R.drawable.beauty_filter_chuwen, "初吻", "CHUWEN",4),
    HUOLI(R.drawable.beauty_filter_huoli, "活力",  "HUOLI",5),
    JIDONG(R.drawable.beauty_filter_jidong, "悸动",  "JIDONG",6),
    LIREN(R.drawable.beauty_filter_liren, "丽人",  "LIREN",7),
    MOREN(R.drawable.beauty_filter_moren, "默认",  "MOREN",8),
    NUANXIN(R.drawable.beauty_filter_nuanxin, "暖心",  "NUANXIN",9),
    QINGCHE(R.drawable.beauty_filter_qingche, "清澈", "QINGCHE",10),
    QINGCHENG(R.drawable.beauty_filter_qingcheng, "清澄", "QINGCHENG",11),
    RILUO(R.drawable.beauty_filter_riluo, "日落", "RILUO",12),
    RIXI(R.drawable.beauty_filter_rixi, "日系",  "RIXI",13),
    SHENMI(R.drawable.beauty_filter_shenmi, "神秘", "SHENMI",14),
    SHISHANG(R.drawable.beauty_filter_shishang, "时尚", "SHISHANG",15),
    SHUIGUANG(R.drawable.beauty_filter_shuiguang, "水光", "SHUIGUANG",16),
    WEIFENG(R.drawable.beauty_filter_weifeng, "微风", "WEIFENG",17),
    WUHOU(R.drawable.beauty_filter_wuhou, "午后", "WUHOU",18),
    XIANGPIAN(R.drawable.beauty_filter_xiangpian, "相片", "XIANGPIAN",19),
    ZIRAN(R.drawable.beauty_filter_ziran, "自然", "ZIRAN",20);

    companion object {
        private val map: MutableMap<Int, LutFilter> = HashMap()
        private val nameMap: MutableMap<String, LutFilter> = HashMap()

        init {
            for (i in LutFilter.values()) {
                map[i.lutValue] = i
                nameMap[i.lutName] = i
            }
        }

        fun fromInt(type: Int?): LutFilter? {
            return map[type]
        }

        fun fromName(name: String): LutFilter? {
            return nameMap[name]
        }
    }
}