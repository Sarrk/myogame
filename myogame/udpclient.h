/*
 * This file contains all the code required to connect to the rpl router via udp
 *
 */


/*
 * @brief  Send data to python game via udp
 * 
 * @param  char *data : data to send
 *
 * @return None
 */
void udp_send(char *data);

/*
 * @brief  Connect to rpl router
 * 
 * @param  None
 * 
 * @return None
 */
void init_udp(void);