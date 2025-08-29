
#include <chrono>

#include "../Headers/BoneHelper.h"
#include "../Headers/EntityDataManager.h"
#include "../Headers/EspHelper.h"
#include "../Headers/FpsCounter.h"
#include "../Headers/Globals.h"
#include "../Headers/HealBarHelper.h"  // Fixed typo
#include "../Headers/Memory.h"
#include "../Headers/Offsets.h"
#include "../Headers/WaterMarkHelper.h"

namespace RenderManager {

void RenderFrame(Memory& mem, uintptr_t client, DataManager& entityManager,
                 ImDrawList* backgroundDraw) {
  if (!backgroundDraw || !client) return;
}
}  // namespace RenderManager