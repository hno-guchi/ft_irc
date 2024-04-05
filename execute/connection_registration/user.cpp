/*
 * Command   : USER
 * Parameters: <user> <mode> <unused> <realname>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             462 ERR_ALREADYREGISTRED ":Unauthorized command (already registered)"
 *
 * Example:
 *    USER guest 0 * :Ronnie Reagan
 *             ; User registering themselves with a username of "guest" and real name "Ronnie Reagan".
 *
 *    USER guest 8 * :Ronnie Reagan
 *             ; User registering themselves with a username of "guest" and real name "Ronnie Reagan", and asking to be set invisible.
 */
