#ifndef CORE_H
#define CORE_H

#include "..\HGE\hge.h"

class Sprite;
class Vector2;

//Don't mess with this unless you're familiar with HGE, or brave.
extern HGE* g_hge;

float GetDeltaTime();	//Returns the time, in seconds.  Timer resolution is 1ms so this will never return less than 0.001f

//System functions. Don't mess with these unless you're familiar with HGE, or brave.
void RenderBegin();
void RenderEnd();
void Initialize(int screenWidthInPixels, int screenHeightInPixels, hgeCallback updateCallback, hgeCallback renderCallback);
bool IsInitialized();
void Run();
void Shutdown();

#endif