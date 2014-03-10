
#include "kalman.h"

void kalman_init(t_kalman_state *state, double q, double r, double p, double intial_value)
{
	state->q = q;
	state->r = r;
	state->p = p;
	state->x = intial_value;
}

void kalman_update(t_kalman_state* state, double measurement)
{
  //prediction update
  //omit x = x
  state->p = state->p + state->q;
 
  //measurement update
  state->k = state->p / (state->p + state->r);
  state->x = state->x + state->k * (measurement - state->x);
  state->p = (1 - state->k) * state->p;
}
