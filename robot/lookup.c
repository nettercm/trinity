#include <stdio.h>

#include "typedefs.h"
#include "lookup.h"


static t_LOOKUP_table t1[] =
{
      {     7  },                   //first x indicates size of the table
      {     -100, 500   },
      {     0     ,     400   },
      {     100   ,     300   },
      {     200   ,     250   },
      {     400   ,     200   },
      {     1000, 0     },
      {     2000, -200}
};


void LOOKUP_initialize_table(t_LOOKUP_table *t)
{
      int i;
      s16 dx1,dy1,slope;

      for(i=1; i<t[0].x; i++)
      {
            dx1 = t[i+1].x - t[i].x;
            dy1 = t[i+1].y - t[i].y;
            slope = (dy1<<6) / dx1; //need to scalce to avoid loss of precision
            t[i].slope = slope;
      }
      t[i].slope = 0; //last slope always 0
}


s16 LOOKUP_do(s16 x, t_LOOKUP_table *t)
{
      int i=0;
      int size;
      s16 dx2,dy2, y;

      size = t[0].x;

      if( x <= t[1].x ) 
      {
            return t[1].y;
      }
      if( x >= t[size].x ) 
      {
            return t[size].y;
      }

      for(i=1; i<size; i++)
      {
            if (x < t[i+1].x)
            {
                  dx2 = x - t[i].x;
                  dy2 = (t[i].slope * dx2)>>6;
                  y   = t[i].y + dy2;
                  return y;
            }
      }
      return t[i].y;
}


void LOOKUP_init(void)
{
	
}


void LOOKUP_test(void)
{
      volatile int y;
      LOOKUP_initialize_table(t1);

      y = LOOKUP_do(-1 , t1);
      y = LOOKUP_do(1500 , t1);
      y = LOOKUP_do(2000 , t1);
      y = LOOKUP_do(3000 , t1);

      y = LOOKUP_do(999, t1);

      y = LOOKUP_do(99,  t1);
      y = LOOKUP_do(98,  t1);
      y = LOOKUP_do(97,  t1);

      y = LOOKUP_do(0  , t1);
      y = LOOKUP_do(1  , t1);
      y = LOOKUP_do(20,  t1);
      y = LOOKUP_do(50,  t1);
      y = LOOKUP_do(90,  t1);
      y = LOOKUP_do(100, t1);
      y = LOOKUP_do(101, t1);
      y = LOOKUP_do(102, t1);
      y = LOOKUP_do(103, t1);

      y = LOOKUP_do(500, t1);

      y = LOOKUP_do(700, t1);

      y = LOOKUP_do(900, t1);
      y = LOOKUP_do(990, t1);
      y = LOOKUP_do(999, t1);

      y = LOOKUP_do(1001, t1);
      y = LOOKUP_do(1111, t1);
}


