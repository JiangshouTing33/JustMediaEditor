//
// Created by 江守亭 on 2021/10/11.
//

#ifndef JUSTMEDIAEDITOR_TEXTDISPLAYPARAM_H
#define JUSTMEDIAEDITOR_TEXTDISPLAYPARAM_H

#include <gtc/matrix_transform.hpp>
#include <cwchar>
#include <cstring>

using namespace glm;

class TextDisplayParam {
public:
	TextDisplayParam(const wchar_t *textStr, const char *fontNameStr, long startTime, long endTime, int textSize,
					 float scale, const vec3 &textColor, const vec2 &displayPosition) :startTime(startTime),
																						endTime(endTime),
																						textSize(textSize),
																						scale(scale),
																						textColor(textColor),
																						displayPosition(displayPosition) {
		wcscpy(text, textStr);
		strcpy(fontName, fontNameStr);
	}

	~TextDisplayParam() {}

public:
	wchar_t text[512];
	char fontName[512];
	long startTime;
	long endTime;
	int textSize;
	float scale;
	glm::vec3 textColor;
	glm::vec2 displayPosition;
};

#endif //JUSTMEDIAEDITOR_TEXTDISPLAYPARAM_H
