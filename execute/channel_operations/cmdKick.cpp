/*
 *    Command: KICK
 * Parameters: <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS    "<command> :Not enough parameters"
 *             403 ERR_NOSUCHCHANNEL     "<channel name> :No such channel"
 *             476 ERR_BADCHANMASK       "<channel> :Bad Channel Mask"
 *             482 ERR_CHANOPRIVSNEEDED  "<channel> :You're not channel operator"
 *             441 ERR_USERNOTINCHANNEL  "<nick> <channel> :They aren't on that channel"
 *             442 ERR_NOTONCHANNEL      "<channel> :You're not on that channel"
 * Examples:
 *    KICK &Melbourne Matthew         ; Command to kick Matthew from &Melbourne
 *    KICK #Finnish John :Speaking English
 *                                    ; Command to kick John from #Finnish using "Speaking English" as the reason (comment).
 *    :WiZ!jto@tolsun.oulu.fi KICK #Finnish John
 *                                    ; KICK message on channel #Finnish from WiZ to remove John from channel
 *
 */
