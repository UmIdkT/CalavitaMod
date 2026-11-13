#include <psp2/ctrl.h>
#include <psp2/display.h>
#include <psp2/kernel/processmgr.h>
#include <vita2d.h>

int main(void) {
    sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG);

    vita2d_init();

    bool running = true;
    while (running) {
        SceCtrlData pad;
        sceCtrlPeekBufferPositive(0, &pad, 1);

        if (pad.buttons & SCE_CTRL_START) {
            running = false;
        }

        vita2d_start_drawing();
        vita2d_clear_screen();

        // Temporary smoke-test graphic; gameplay will be added later.
        vita2d_draw_rectangle(70.0f, 70.0f, 500.0f, 300.0f, RGBA8(35, 90, 145, 255));
        vita2d_draw_rectangle(90.0f, 90.0f, 460.0f, 8.0f, RGBA8(230, 190, 80, 255));

        vita2d_end_drawing();
        vita2d_swap_buffers();
        sceDisplayWaitVblankStart();
    }

    vita2d_fini();
    sceKernelExitProcess(0);
    return 0;
}