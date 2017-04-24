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

This message has a link to some cool stuff in it.

--aaa
Content-Type: text/html

<html>
<head>
</head>
<body>
X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*
<ul>
  <li>This</li>
  <li>is</li>
  <li>a</li>
  <li>list</li>
  <li>X5O!P%@AP[4\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*</li>
</ul>
</body>
<html>

--aaa--

.
QUIT

