/*
 *    Command: PART
 * Parameters: <channel> *( "," <channel> ) [ <Part Message> ]
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS  "<command> :Not enough parameters"
 *             403 ERR_NOSUCHCHANNEL   "<channel name> :No such channel"
 *             442 ERR_NOTONCHANNEL    "<channel> :You're not on that channel"
 *
 * Examples:
 *    PART #twilight_zone             ; Command to leave channel "#twilight_zone"
 *    PART #oz-ops,&group5            ; Command to leave both channels "&group5" and "#oz-ops".
 *    :WiZ!jto@tolsun.oulu.fi PART #playzone :I lost
 *                                    ; User WiZ leaving channel "#playzone" with the message "I lost".
 */
