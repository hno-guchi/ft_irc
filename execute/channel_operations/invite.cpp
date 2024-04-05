/*
 *    Command: INVITE
 * Parameters: <nickname> <channel>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS    "<command> :Not enough parameters"
 *             401 ERR_NOSUCHNICK        "<nickname> :No such nick/channel"
 *             442 ERR_NOTONCHANNEL      "<channel> :You're not on that channel"
 *             443 ERR_USERONCHANNEL     "<user> <channel> :is already on channel"
 *             482 ERR_CHANOPRIVSNEEDED  "<channel> :You're not channel operator"
 *             341 RPL_INVITING          "<channel> <nick>"
 *             301 RPL_AWAY              "<nick> :<away message>"
 * Examples:
 *    :Angel!wings@irc.org INVITE Wiz #Dust
 *                                    ; Message to WiZ when he has been invited by user Angel to channel #Dust
 *    INVITE Wiz #Twilight_Zone       ; Command to invite WiZ to #Twilight_zone
 */
