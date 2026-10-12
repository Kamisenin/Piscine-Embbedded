#include "game.h"

extern volatile t_state status;

void on_slave_disconnect(void)
{
    stop_game();
}

void on_slave_not_recieve() //since the only error case of not recieving is slave not being connected in our program, we call the function to not have to code it again
{
    on_slave_disconnect();
}

void on_master_bus_error() // same as on_slave_not_recieve
{
    stop_game();
}