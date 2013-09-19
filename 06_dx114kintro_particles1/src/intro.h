#ifndef INTRO_HPP
#define INTRO_HPP

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <D3D11.h>
#include <D3DX11.h>
#include <D3DCompiler.h>
#include "fp.h"
#include "config.h"
#include "4klang.h"
#include "mmsystem.h"

void IntroInit();
void IntroLoop(long sample);

#endif