#include "key.h"
#include "logic.h"
#include "ALLNeed.h"

#define KEY_DEBOUNCE_MS 20U

/* Existing route reset in MDK-ARM/logic.c. */
extern void all_init(void);

typedef struct {
    uint8_t sampled;
    uint8_t stable;
    uint32_t changed_at;
} KeyState;

static KeyState keys[4];
static uint8_t active_task;

static uint8_t Key_Pressed(KeyState *state, uint8_t pressed, uint32_t now)
{
    if (pressed != state->sampled) {
        state->sampled = pressed;
        state->changed_at = now;
    } else if (pressed != state->stable &&
               (uint32_t)(now - state->changed_at) >= KEY_DEBOUNCE_MS) {
        state->stable = pressed;
        return pressed;
    }

    return 0U;
}

void Key_Poll(void)
{
    uint32_t now = HAL_GetTick();
    const uint8_t pressed[] = {key1, key2, key3, key4};
    uint8_t selected = 0U;
    uint8_t i;

    for (i = 0U; i < 4U; ++i) {
        if (Key_Pressed(&keys[i], pressed[i], now) && selected == 0U) {
            selected = i + 1U;
        }
    }

    if (selected != 0U) {
        aaa = 0;
        all_init();
        active_task = selected;
    }

    switch (active_task) {
    case 1U: Test1(); break;
    case 2U: Test2(); break;
    case 3U: Test3(); break;
    case 4U: Test4(); break;
    default: break;
    }
}
