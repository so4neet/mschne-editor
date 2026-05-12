#include <raylib.h>
#include <raymath.h>
#include <math.h>
#include <stdio.h>

static Vector2 lastMousePos = { 0 };

void InitEditorCamera(Camera *cam, float *angleX, float *angleY) {
        cam->position   = (Vector3) { 15.0f, 15.0f, 15.0f };
        cam->target     = (Vector3) {0.0f, 0.0f, 0.0f};
        cam->up         = (Vector3) {0.0f, 1.0f, 0.0f};
        cam->fovy       = 60.0f;
        cam->projection = CAMERA_PERSPECTIVE;

        Vector3 lookDir = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
        *angleX = atan2f(lookDir.x, lookDir.z);
        *angleY = asinf(lookDir.y);
}

void UpdateEditorCamera(Camera *cam, float *angleX, float *angleY, bool isTyping, bool isMouseOverUI) {
        if (isTyping) return;

        float moveSpeed = IsKeyDown(KEY_LEFT_SHIFT) ? 0.2f : 0.05f;
        Vector3 forward         = Vector3Normalize(Vector3Subtract(cam->target, cam->position));
        Vector3 right           = Vector3Normalize(Vector3CrossProduct(forward, cam->up));
        Vector3 groundForward   = Vector3Normalize((Vector3){forward.x, 0, forward.z});

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) lastMousePos = GetMousePosition();
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector2 delta = Vector2Subtract(GetMousePosition(), lastMousePos);
                lastMousePos = GetMousePosition();
                *angleX -= delta.x * 0.003f;
                *angleY -= delta.y * 0.003f;
                *angleY = Clamp(*angleY, -1.5f, 1.5f);
                Vector3 dir = {
                        cosf(*angleY) * sinf(*angleX),
                        sinf(*angleY),
                        cosf(*angleY) * cosf(*angleX)
                };
                cam->target = Vector3Add(cam->position, dir);
                printf("%f, %f, %f\n", cam->target.x, cam->target.y, cam->target.z);
        }

        if (IsKeyDown(KEY_W)) {cam->position = Vector3Add(cam->position, Vector3Scale(groundForward, moveSpeed)); cam->target = Vector3Add(cam->target, Vector3Scale(groundForward, moveSpeed)); }
        if (IsKeyDown(KEY_S)) {cam->position = Vector3Add(cam->position, Vector3Scale(groundForward, -moveSpeed)); cam->target = Vector3Add(cam->target, Vector3Scale(groundForward, -moveSpeed)); }
        if (IsKeyDown(KEY_A)) { cam->position = Vector3Add(cam->position, Vector3Scale(right, -moveSpeed)); cam->target = Vector3Add(cam->target, Vector3Scale(right, -moveSpeed)); }
        if (IsKeyDown(KEY_D)) { cam->position = Vector3Add(cam->position, Vector3Scale(right,  moveSpeed)); cam->target = Vector3Add(cam->target, Vector3Scale(right,  moveSpeed)); }
        if (IsKeyDown(KEY_E)) { cam->position.y += moveSpeed; cam->target.y += moveSpeed; }
        if (IsKeyDown(KEY_Q)) { cam->position.y -= moveSpeed; cam->target.y -= moveSpeed; }
}