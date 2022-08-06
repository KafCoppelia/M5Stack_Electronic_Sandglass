#include "../include/sandglass2.h"
#include "../include/ledmatrix2.h"
#include "../include/matrixsand.h"
#include <M5StickCPlus.h>

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

static void update_matrix(LedMatrix2 *m, matrix_sand::MatrxiSand *s);

void update_matrix(LedMatrix2 *m, matrix_sand::MatrxiSand *s) {
    int i, j;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            m->__setitem__(i, j, s->__getitem__(i, j));
        }
    }
}

void test(void) {
    matrix_sand::MatrxiSand sand1 = matrix_sand::MatrxiSand(8, 8);
    matrix_sand::MatrxiSand sand2 = matrix_sand::MatrxiSand(8, 8);

    LedMatrix2 m1 = LedMatrix2(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false);
    LedMatrix2 m2 = LedMatrix2(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false);

    unsigned long _start_tick, _now_tick;
    int i, j;
    int x, y;
    for (i = 0; i < 8; i++) {
        sand1.__setitem__(i, i, true);
    }

    sand2.__setitem__(0, 0, true);
    sand2.__setitem__(1, 1, true);
    sand2.__setitem__(2, 2, true);
    sand2.__setitem__(3, 3, true);
    sand2.__setitem__(4, 4, true);
    sand2.__setitem__(5, 5, true);
    sand2.__setitem__(6, 6, true);
    sand2.__setitem__(7, 7, true);

    // update_matrix(m1, sand1);
    update_matrix(&m2, &sand2);
    m1.show();
    // m1.fill();
    // m2.fill();

    M5.Lcd.setCursor(10, 15);
    for (i = 0; i < 8; i++) {
        M5.Lcd.printf("%d ", sand2.__getitem__(i, i));
    }

    M5.Lcd.setCursor(10, 30);
    for (i = 0; i < 7; i++) {
        M5.Lcd.printf("0x%02X ", m2._get_line(i));
    }

    for (i = 0; i < 40; i++) {
        m1.show();
        m2.show();
        // delay(10);
    }

    m1.fill(0);
    m2.fill(0);
    m1.show();

    _start_tick = millis();
    while (1) {
        M5.Lcd.setCursor(10, 15);
        for (i = 0; i < 7; i++) {
            M5.Lcd.printf("0x%02X ", m2._get_line(i));
        }

        _now_tick = millis();
        if ((_now_tick - _start_tick) >= (unsigned long)(0.5 * 1000)) {
            x = rand()%8;
            y = rand()%8;
            m2.__setitem__(x , y, 1 - m2.__getitem__(x, y));
            _start_tick = _now_tick;
        }
        m2.show();
    }
}

void common_test(void) {
    float accX, accY, accZ;
    float xx, yy, zz;
    int sx, sy;
    int count = 0;
    bool updated1, updated2;
    unsigned long _start_tick, _now_tick;

    matrix_sand::MatrxiSand sand1 = matrix_sand::MatrxiSand(8, 8);
    matrix_sand::MatrxiSand sand2 = matrix_sand::MatrxiSand(8, 8);

    LedMatrix2 m1 = LedMatrix2(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1, false);
    LedMatrix2 m2 = LedMatrix2(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2, false);
    
    for (sx = 0; sx < 8; sx++) {
        for (sy = 0; sy < 8; sy++) {
            sand1.__setitem__(sx, sy, true);
        }
    }

    sand1.__setitem__(0, 0, false);
    sand1.__setitem__(0, 1, false);
    sand1.__setitem__(1, 0, false);
    sand1.__setitem__(0, 2, false);
    sand1.__setitem__(1, 1, false);
    sand1.__setitem__(2, 0, false);

    update_matrix(&m1, &sand1);
    update_matrix(&m2, &sand2);

    m1.show();
    m2.show();

    updated1 = updated2 = false;

    _start_tick = millis();
    while (1) {
        _now_tick = millis();
        if ((_now_tick - _start_tick) >= (unsigned long)(1 * 1000)) {
            M5.IMU.getAccelData(&accX, &accY, &accZ);
            M5.Lcd.setCursor(10, 45);
            M5.Lcd.printf("X:%5.2f/nY:%5.2f/nZ:%5.2f ", accX, accY, accZ);

            xx = -accZ - accY;
            yy = -accZ + accY;
            zz = accX;

            if (yy > 0 and sand1.__getitem__(7, 7) and not sand2.__getitem__(0, 0) and not updated2) {
                sand1.__setitem__(7, 7, false);
                sand2.__setitem__(0, 0, true);
                updated1 = updated2 = true;
            } 
            else if (yy <= 0 and sand2.__getitem__(0, 0) and not sand1.__getitem__(7, 7) and not updated1) {
                sand1.__setitem__(7, 7, true);
                sand2.__setitem__(0, 0, false);
                updated1 = updated2 = true;
            } 
            else {
                updated1 = sand1.iterate(xx, yy, zz);
                updated2 = sand2.iterate(xx, yy, zz);
            }

            if (updated1) {
                update_matrix(&m1, &sand1);
            }
            if (updated2) {
                update_matrix(&m2, &sand2);
            }
            _start_tick = _now_tick;
        }
        
        // For debugging
        M5.Lcd.setCursor(10, 15);
        for (int i = 0; i < 7; i++) {
            M5.Lcd.printf("0x%02X ", m1._get_line(i));
        }

        m1.show();
        m2.show();
    }
}