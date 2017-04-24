HELO 192.168.1.30
MAIL FROM: Guest<spammer@example.org>
RCPT TO: <kumar@acme.com>
DATA
From: Guest<spammer@example.org>
To: Host Master<kumar@acme.com>
Subject: attachment test
MIME-Version: 1.0
Content-Type: multipart/mixed;
 boundary="aaa"

--aaa
Content-Type: text/plain;
 charset="iso-8859-1"

This message has a text attachment named "HelloWorld.txt"

--aaa
Content-Type: text/plain;
 name="HelloWorld.txt"
Content-Disposition: attachment;
 filename="HelloWorld.txt"

--aaa--

.
QUIT

