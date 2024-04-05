/*
 * Command   : MODE
 * Parameters: <nickname> *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             502 ERR_USERSDONTMATCH   ":Cannot change mode for other users"
 *             501 ERR_UMODEUNKNOWNFLAG ":Unknown MODE flag"
 *             221 RPL_UMODEIS          "<user mode string>"
 *
 * Examples:
 *    MODE WiZ -w
 *             ; Command by WiZ to turn off reception of WALLOPS messages.
 *
 *    MODE Angel +i
 *             ; Command from Angel to make herself invisible.
 *
 *    MODE WiZ -o
 *             ; WiZ 'deopping' (removing operator status).
 */
