#ifndef PRE_COMPILED_HEADER
#define PRE_COMPILED_HEADER

#pragma region Standart
#include <iostream>
#include <string>
#include <sstream>
#include <math.h>
#include <fstream>
#include <random>
#include <stdio.h>
#include <vector>
#include <list>
#include <queue>
#include <algorithm>
#include <functional>
#include <memory>
#include <set>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include <new>

#define SAFE_DELETE(p) if (p) { delete (p); (p) = nullptr; }

#ifndef	_WIN32
#include <unistd.h>
#endif

#if _DEBUG
#define USE_VLD
#include <VLD/vld.h>
#endif
#pragma endregion

#pragma region Third Party
#include <GL/gl3w.h>
#include <ImGui/imgui.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include "EliteMath/EMath.h"
#include "EliteInput/EInputCodes.h"
#include "EliteInput/EInputData.h"
#pragma endregion

#endif