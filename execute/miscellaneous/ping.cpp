/*
 * Command   : PING
 * Parameters: <server1> [ <server2> ]
 *
 * Numeric Replies:
 *             409 ERR_NOORIGIN     ":No origin specified"
 *             402 ERR_NOSUCHSERVER "<server name> :No such server"
 *
 * Examples:
 *    PING tolsun.oulu.fi             ; Command to send a PING message to server
 *    PING WiZ tolsun.oulu.fi         ; Command from WiZ to send a PING message to server "tolsun.oulu.fi"
 *    PING :irc.funet.fi              ; Ping message sent by server "irc.funet.fi"
 */
