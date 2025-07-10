#ifndef SWITCH_H
#define SWITCH_H

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*switch_callback_t)(int switch_id);

void switch_init(switch_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // SWITCH_H
