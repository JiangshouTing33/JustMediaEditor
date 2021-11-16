package just.android.justmediaeditor.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import just.android.justmediaeditor.R
import just.android.mediaeditor.param.FontTextOption

class FontTextListAdapter(private val textOptionList: MutableList<FontTextOption>):
    RecyclerView.Adapter<FontTextListAdapter.FontTextViewHolder>() {

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): FontTextViewHolder = FontTextViewHolder.create(parent)

    override fun onBindViewHolder(holder: FontTextViewHolder, position: Int) {
        holder.bind(textOptionList[position])
        holder.tvFontTextItemDelete.setOnClickListener {
            textOptionList.removeAt(position)
            notifyDataSetChanged()
        }
    }

    override fun getItemCount(): Int = textOptionList.size

    class FontTextViewHolder(itemView: View) : RecyclerView.ViewHolder(itemView) {
        var tvFontTextItemDelete: TextView = itemView.findViewById(R.id.tv_font_text_item_delete)

        private var tvFontItemText: TextView = itemView.findViewById(R.id.tv_font_item_text)
        private var tvFontTextItemDuration: TextView = itemView.findViewById(R.id.tv_font_text_item_duration)

        companion object {
            fun create(parent: ViewGroup): FontTextViewHolder =
                FontTextViewHolder(
                    LayoutInflater.from(parent.context)
                        .inflate(R.layout.adapter_item_font_text, parent, false)
                )
        }

        fun bind(fontTextOption: FontTextOption) {
            tvFontItemText.text = fontTextOption.text
            tvFontTextItemDuration.text = String.format(itemView.context.getString(R.string.duration_value),
                fontTextOption.durationStart, fontTextOption.durationEnd)
        }
    }
}