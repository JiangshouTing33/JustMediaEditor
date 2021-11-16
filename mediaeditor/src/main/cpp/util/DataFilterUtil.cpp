//
// Created by 江守亭 on 2021/10/11.
//

#include "DataFilterUtil.h"
bool DataFilterUtil::TextDisplayParamFilter(TextDisplayParam *param) {
	if (wcslen(param->text) == 0) {
		return false;
	}

	if (strlen(param->fontName) == 0) {
		return false;
	}

	return param->startTime <= param->endTime;
}

void DataFilterUtil::SortTextDisplayParams(std::deque<TextDisplayParam *> *params) {
	sort(params->begin(), params->end(), TextDisplayParamCompare);
}

bool DataFilterUtil::TextDisplayParamCompare(const TextDisplayParam *param1, const TextDisplayParam *param2) {
	return param1->startTime < param2->startTime;
}

bool DataFilterUtil::StickerDisplayOptionFilter(const StickerDisplayOption& option) {
	return option.startTime <= option.endTime;
}

void DataFilterUtil::SortStickerDisplayOptions(std::deque<StickerDisplayOption> *option) {
	sort(option->begin(), option->end(), StickerDisplayOptionCompare);
}

bool DataFilterUtil::StickerDisplayOptionCompare(const StickerDisplayOption& option1, const StickerDisplayOption& option2) {
	return option1.startTime < option2.startTime;
}