/*
 *    Command: OPER
 * Parameters: <name> <password>
 *
 * Numeric Replies:
 *             461 ERR_NEEDMOREPARAMS   "<command> :Not enough parameters"
 *             381 RPL_YOUREOPER        ":You are now an IRC operator"
 *             491 ERR_NOOPERHOST       ":No O-lines for your host"
 *             464 ERR_PASSWDMISMATCH   ":Password incorrect"
 * Example:
 *    OPER foo bar
 *             ; Attempt to register as an operator using a username of "foo" and "bar" as the password.
 */
