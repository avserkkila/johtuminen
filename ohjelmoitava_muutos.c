#include "askellus.h"
#include "cfgmuuttujat.h"
#include "grid.h"

void ohjelmoitava_muutos() {
  //return;
  static unsigned lasku = 0;
  if( lasku++ < 5000)
    return;
  if( lasku % 1000 )
    return;
  jakosrt++;
  grid_jako();
  grid_Treun_2d(grid.g);
  grid_Treun_2d(grid.g2);
}
