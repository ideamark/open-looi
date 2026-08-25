#include "animation.h"

int selection = 0;

void play_single_animation(const byte frames[][512], int frame_count, int loop_count = 3) {
  int frame = 0;
  for (int i = 0; i < loop_count; i++) {
    while (true) {
        frame = (frame + 1) % frame_count;
        if (frame == 0) {
            break;
        }
        display.clearDisplay();
        display.drawBitmap(32, 0, frames[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.display();
        delay(FRAME_DELAY);
    }
  }
  delay(300);
  eye_blink();
}

void play_animation(int num) {
    int frame_count;
    switch (num) {
        case 0:
            frame_count = (sizeof(frames0) / sizeof(frames0[0]));
            play_single_animation(frames0, frame_count);
            break;
        case 1:
            frame_count = (sizeof(frames1) / sizeof(frames1[0]));
            play_single_animation(frames1, frame_count);
            break;
        case 2:
            frame_count = (sizeof(frames2) / sizeof(frames2[0]));
            play_single_animation(frames2, frame_count);
            break;
        case 3:
            frame_count = (sizeof(frames3) / sizeof(frames3[0]));
            play_single_animation(frames3, frame_count);
            break;
        case 4:
            frame_count = (sizeof(frames4) / sizeof(frames4[0]));
            play_single_animation(frames4, frame_count);
            break;
        case 5:
            frame_count = (sizeof(frames5) / sizeof(frames5[0]));
            play_single_animation(frames5, frame_count);
            break;
        case 6:
            frame_count = (sizeof(frames6) / sizeof(frames6[0]));
            play_single_animation(frames6, frame_count);
            break;
        case 7:
            frame_count = (sizeof(frames7) / sizeof(frames7[0]));
            play_single_animation(frames7, frame_count);
            break;
        case 8:
            frame_count = (sizeof(frames8) / sizeof(frames8[0]));
            play_single_animation(frames8, frame_count);
            break;
        case 9:
            frame_count = (sizeof(frames9) / sizeof(frames9[0]));
            play_single_animation(frames9, frame_count);
            break;
        case 10:
            frame_count = (sizeof(frames10) / sizeof(frames10[0]));
            play_single_animation(frames10, frame_count);
            break;
        case 11:
            frame_count = (sizeof(frames11) / sizeof(frames11[0]));
            play_single_animation(frames11, frame_count);
            break;
        case 12:
            frame_count = (sizeof(frames12) / sizeof(frames12[0]));
            play_single_animation(frames12, frame_count);
            break;
        case 13:
            frame_count = (sizeof(frames13) / sizeof(frames13[0]));
            play_single_animation(frames13, frame_count);
            break;
        case 14:
            frame_count = (sizeof(frames14) / sizeof(frames14[0]));
            play_single_animation(frames14, frame_count);
            break;
        case 15:
            frame_count = (sizeof(frames15) / sizeof(frames15[0]));
            play_single_animation(frames15, frame_count);
            break;
        case 16:
            frame_count = (sizeof(frames16) / sizeof(frames16[0]));
            play_single_animation(frames16, frame_count);
            break;
        case 17:
            frame_count = (sizeof(frames17) / sizeof(frames17[0]));
            play_single_animation(frames17, frame_count);
            break;
        case 18:
            frame_count = (sizeof(frames18) / sizeof(frames18[0]));
            play_single_animation(frames18, frame_count);
            break;
        case 19:
            frame_count = (sizeof(frames19) / sizeof(frames19[0]));
            play_single_animation(frames19, frame_count);
            break;
        case 20:
            frame_count = (sizeof(frames20) / sizeof(frames20[0]));
            play_single_animation(frames20, frame_count);
            break;
        case 21:
            frame_count = (sizeof(frames21) / sizeof(frames21[0]));
            play_single_animation(frames21, frame_count);
            break;
        case 22:
            frame_count = (sizeof(frames22) / sizeof(frames22[2]));
            play_single_animation(frames22, frame_count);
            break;
        case 23:
            frame_count = (sizeof(frames23) / sizeof(frames23[3]));
            play_single_animation(frames23, frame_count);
            break;
        case 24:
            frame_count = (sizeof(frames24) / sizeof(frames24[4]));
            play_single_animation(frames24, frame_count);
            break;
        case 25:
            frame_count = (sizeof(frames25) / sizeof(frames25[5]));
            play_single_animation(frames25, frame_count);
            break;
        case 26:
            frame_count = (sizeof(frames26) / sizeof(frames26[6]));
            play_single_animation(frames26, frame_count);
            break;
        case 27:
            frame_count = (sizeof(frames27) / sizeof(frames27[7]));
            play_single_animation(frames27, frame_count);
            break;
        case 28:
            frame_count = (sizeof(frames28) / sizeof(frames28[8]));
            play_single_animation(frames28, frame_count);
            break;
        case 29:
            frame_count = (sizeof(frames29) / sizeof(frames29[9]));
            play_single_animation(frames29, frame_count);
            break;
        case 30:
            frame_count = (sizeof(frames30) / sizeof(frames30[0]));
            play_single_animation(frames30, frame_count);
            break;
        case 31:
            frame_count = (sizeof(frames31) / sizeof(frames31[1]));
            play_single_animation(frames31, frame_count);
            break;
        case 32:
            frame_count = (sizeof(frames32) / sizeof(frames32[2]));
            play_single_animation(frames32, frame_count);
            break;
        case 33:
            frame_count = (sizeof(frames33) / sizeof(frames33[3]));
            play_single_animation(frames33, frame_count);
            break;
        case 34:
            frame_count = (sizeof(frames34) / sizeof(frames34[4]));
            play_single_animation(frames34, frame_count);
            break;
        case 35:
            frame_count = (sizeof(frames35) / sizeof(frames35[5]));
            play_single_animation(frames35, frame_count);
            break;
        case 36:
            frame_count = (sizeof(frames36) / sizeof(frames36[6]));
            play_single_animation(frames36, frame_count);
            break;
        case 37:
            frame_count = (sizeof(frames37) / sizeof(frames37[7]));
            play_single_animation(frames37, frame_count);
            break;
        case 38:
            frame_count = (sizeof(frames38) / sizeof(frames38[8]));
            play_single_animation(frames38, frame_count);
            break;
        case 39:
            frame_count = (sizeof(frames39) / sizeof(frames39[9]));
            play_single_animation(frames39, frame_count);
            break;
        case 40:
            frame_count = (sizeof(frames40) / sizeof(frames40[0]));
            play_single_animation(frames40, frame_count);
            break;
        case 41:
            frame_count = (sizeof(frames41) / sizeof(frames41[0]));
            play_single_animation(frames41, frame_count);
            break;
        default:  
            return;
    }
    selection++;
    if (selection > FRAMES_COUNT) {
        selection = 0;
    } else if (selection < 0) {
        selection = FRAMES_COUNT;
    }
}
