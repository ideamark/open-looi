#ifndef Act_h
#define Act_h

#include "common.h"
#include "emoji.h"
#include "head.h"
#include "cmd.h"

void sequent_act(String actions);
void random_act();
void speech_act();
void async_sequent_act(String actions);
void async_random_act();
void async_speech_act();

#endif