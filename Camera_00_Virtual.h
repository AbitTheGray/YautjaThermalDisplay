#pragma once
#include "DataStructs.h"

//----------------------------------------------------------------
// Main functions 

void setupMlx90640()
{
  
}

void loadTemps(Temp& temp)
{
  // Random data
  for(int y = 0; y < Temps_Height; y++)
  {
    int yi = y * Temps_Width;
    for(int x = 0; x < Temps_Width; x++)
    {
      int i = yi + x;

      temp.Values[i] = random(temp.Min * 100, temp.Max * 100) / 100.0f;
    }
  }
}
