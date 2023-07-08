#pragma once
#include <stdio.h>
#include <stdlib.h>

#include "src/fontx.h"

// Supported Fonts:
extern FontxFile fx16G[2];
extern FontxFile fx24G[2];
extern FontxFile fx32G[2];
extern FontxFile fx32L[2];

extern FontxFile fx16M[2];
extern FontxFile fx24M[2];
extern FontxFile fx32M[2];

void eduboard_init_spiffs(void);