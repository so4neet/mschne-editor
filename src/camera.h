#pragma once
#include <raylib.h>
#include <raymath.h>

void InitEditorCamera(Camera *cam, float *angleX, float *angleY);
void UpdateEditorCamera(Camera *cam, float *angleX, float *angleY, bool isTyping, bool isMouseOverUI);