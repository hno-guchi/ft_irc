# ft_irc

42のカリキュラムにある課題です。C++ を使用して、基本的な IRC プロトコルに沿った通信を処理する簡単な**IRC（Internet Relay Chat）サーバー** を実装しました。

## 機能
- 複数クライアントの接続機能
- チャンネル作成機能
- クライアントのニックネーム設定機能
- クライアントの認証（パスワード認証）機能
- 基本的なコマンド(**RFC 2812**準拠)の実装:
  - CONNECTION_REGISTRATION
    - `PASS` `NICK` `USER` `OPER` `QUIT` `MODE`
  - CHANNEL_OPERATIONS
    - `MODE` `INVITE` `JOIN` `KICK` `PART` `TOPIC`
  - MISCELLANEOUS
    - `PONG`
  - SENDING
    - `NOTICE` `PRIVMSG`

- 参考プロトコル(**RFC 2812**準拠, 一部非対応)
  ```
  // 拡張BNF記法
  message	= [ ":" prefix SPACE ] command [ params ] crlf

  prefix	=	servername / ( nickname [ [ "!" user ] "@" host ] )
  SPACE	=	%x20				; space character
  command	=	1*letter // command	=	1*letter / 3digit
  params	=	*14( SPACE middle ) [ SPACE ":" trailing ]
  			=/	14( SPACE middle ) [ SPACE [ ":" ] trailing ]
  crlf		=	%x0D %x0A			; "carriage return" "linefeed"

  channel		=	"#" *50( chanstring )	// channel = ( "#" / "+" / ( "!" channelid ) / "&" ) chanstring [ ":" chanstring ]
  servername	=	hostname
  nickname		=	( letter / special ) *8( letter / digit / special / "-" )
  user			=	1*( %x01-09 / %x0B-0C / %x0E-1F / %x21-3F / %x41-FF )	; any octet except NUL, CR, LF, " " and "@"
  host			=	hostname			// host = hostname / hostaddr


  hostname		=	shortname *( "." shortname ) // 最大６３文字。これを超える場合は、<hostaddr>を使用する。
  shortname	=	( letter / digit ) *( letter / digit / "-" ) *( letter / digit )	; as specified in RFC 1123 [HNAME]


  letter	=	%x41-5A / %x61-7A		; A-Z / a-z
  digit	=	%x30-39				; 0-9
  special	=	%x5B-60 / %x7B-7D	; "[", "]", "\", "`", "_", "^", "{", "|", "}"

  middle	=	nospcrlfcl *( ":" / nospcrlfcl )
  trailing	=	*( ":" / " " / nospcrlfcl )

  nospcrlfcl	=	%x01-09 / %x0B-0C / %x0E-1F / %x21-39 / %x3B-FF
  					; any octet except NUL, CR, LF, " " and ":"

  chanstring	=	%x01-07 / %x08-09 / %x0B-0C / %x0E-1F / %x21-2B
  chanstring	=/	%x2D-39 / %x3B-FF	; any octet except NUL, BELL, CR, LF, " ", "," and ":"
  key			=	1*23( %x01-05 / %x07-08 / %x0C / %x0E-1F / %x21-7F )
  					; any 7-bit US_ASCII character,
  					; except NUL, CR, LF, FF, h/v TABs, and " "



  hostname
  最大６３文字。これを超える場合は、<hostaddr>を使用する。

  <channel>
  prefixは"#"のみ
  最大５０文字
  チャンネル名に、スペース(「　」)、コントロールG(^GまたはASCII code 7)、カンマ(「 , 」)、ダブルころん（":"）を含んではならない
  チャンネル名は大文字、小文字を区別しない

  CONNECTION_REGISTRATION
  PASS <password>
  NICK <nickname>
  USER <username> <hostname> <servername> <realname>
  OPER <name> <password>
  QUIT [ <Quit Message> ]
  MODE <nickname> ( "+" / "-" ) ( "o" )				// MODE <nickname> *( ( "+" / "-" ) *( "i" / "w" / "o" / "O" / "r" ) )

  CHANNEL_OPERATIONS
  MODE <channel> ( "-" / "+" ) <mode> [ <modeparam> ]	// MODE <channel> *( ( "-" / "+" ) *<modes> *<modeparams> )
  INVITE <nickname> <channel>
  JOIN <channel> [ <key> ]								// JOIN ( <channel> *( "," <channel> ) [ <key> *( "," <key> ) ] ) / "0"
  KICK <channel> <user> [<comment>]					// KICK <channel> *( "," <channel> ) <user> *( "," <user> ) [<comment>]
  PART <channel> [ <Part Message> ]					// PART <channel> *( "," <channel> ) [ <Part Message> ]
  TOPIC <channel> [ <topic> ]

  MISCELLANEOUS
  PING <server1> [ <server2> ]
  PONG <server1> [ <server2> ]

  SENDING
  NOTICE <msgtarget> <text>
  PRIVMSG <msgtarget> <text>


## 必要環境
- C++ 98 準拠のコンパイラ（`g++` など）
- `make`
- `nc`（Netcat を使用してクライアント動作を確認）

## コンパイル
```sh
make
```

## サーバの起動
```sh
./ircserv <ポート番号> <パスワード>
```

## ncコマンドを使用した接続
```sh
nc localhost <ポート番号>
```
