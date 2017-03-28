<?php
/***************************************************************************
 *                              smtp.php
 *                       -------------------
 *   begin                : Wed May 09 2001
 *   copyright            : (C) 2001 The phpBB Group
 *   email                : support@phpbb.com
 *
 *   phpBB  Id: smtp.php,v 1.16.2.9 2003/07/18 16:34:01 acydburn
 *      G2 $Id: smtp.php 20954 2009-12-14 20:10:04Z mindless $
 *
 ***************************************************************************/

/***************************************************************************
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 ***************************************************************************/

function server_parse(&$socket, $response) {
    for ($server_response = ''; substr($server_response, 3, 1) != ' ';) {
	if (!($server_response = fgets($socket, 256))) {
	    return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
					"Couldn't get mail server response code");
	}
    }
    if (!(substr($server_response, 0, 3) == $response)) {
	return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
				    "Problem sending mail. Server response: $server_response");
    }
    return null;
}

function smtpmail($config, $to, $subject, $body, $headers=null) {
    // Fix any bare linefeeds in the message to make it RFC821 Compliant.
    $body = preg_replace("#(?<!\r)\n#si", "\r\n", $body);

    $cc = $bcc = array();
    if (isset($headers)) {
	$headers = rtrim($headers);

	// Make sure there are no bare linefeeds in the headers
	$headers = preg_replace('#(?<!\r)\n#si', "\r\n", $headers);

	if (preg_match('#^cc:\s*(.*?)\s*$#mi', $headers, $match)) {
	    $cc = preg_split('/, */', $match[1]);
	}
	if (preg_match('#^bcc:\s*(.*?)\s*$#mi', $headers, $match)) {
	    $bcc = preg_split('/, */', $match[1]);
	    $headers = preg_replace('#^bcc:.*$#mi', '', $headers);
	}
    }

    if (trim($subject) == '') {
	return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
				    "No email Subject specified");
    }

    if (trim($body) == '') {
	return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
				    "Email message was blank");
    }

    // Connect
    list ($config['smtp.host'], $port) = array_merge(explode(':', $config['smtp.host']), array(25));
    if (!($socket = fsockopen($config['smtp.host'], $port, $errno, $errstr, 20))) {
	return GalleryCoreApi::error(ERROR_PLATFORM_FAILURE, __FILE__, __LINE__,
				    "Could not connect to smtp host : $errno : $errstr");
    }

    // Wait for reply
    $ret = server_parse($socket, "220");
    if ($ret) {
	return $ret;
    }

    // Do we want to use AUTH?, send RFC2554 EHLO, else send RFC821 HELO
    if (!empty($config['smtp.username']) && !empty($config['smtp.password'])) {
	fputs($socket, "EHLO " . $config['smtp.host'] . "\r\n");
	$ret = server_parse($socket, "250");
	if ($ret) {
	    return $ret;
	}

	fputs($socket, "AUTH LOGIN\r\n");
	$ret = server_parse($socket, "334");
	if ($ret) {
	    return $ret;
	}

	fputs($socket, base64_encode($config['smtp.username']) . "\r\n");
	$ret = server_parse($socket, "334");
	if ($ret) {
	    return $ret;
	}

	fputs($socket, $config['smtp.password'] . "\r\n"); // Already encoded
	$ret = server_parse($socket, "235");
	if ($ret) {
	    return $ret;
	}
    } else {
	fputs($socket, "HELO " . $config['smtp.host'] . "\r\n");
	$ret = server_parse($socket, "250");
	if ($ret) {
	    return $ret;
	}
    }

    // From this point onward most server response codes should be 250
    // Specify who the mail is from....
    fputs($socket, "MAIL FROM: <" . $config['smtp.from'] . ">\r\n");
    $ret = server_parse($socket, "250");
    if ($ret) {
	return $ret;
    }

    // Add an additional bit of error checking to the To field.
    $to = (trim($to) == '') ? 'Undisclosed-recipients:;' : trim($to);
    if (preg_match('#[^ ]+\@[^ ]+#', $to)) {
	fputs($socket, "RCPT TO: <$to>\r\n");
	$ret = server_parse($socket, "250");
	if ($ret) {
	    return $ret;
	}
    }

    // Ok now do the CC and BCC fields...
    foreach (array_merge($cc, $bcc) as $address) {
	$address = trim($address);
	if (preg_match('#[^ ]+\@[^ ]+#', $address)) {
	    fputs($socket, "RCPT TO: <$address>\r\n");
	    $ret = server_parse($socket, "250");
	    if ($ret) {
		return $ret;
	    }
	}
    }

    // Ok now we tell the server we are ready to start sending data
    fputs($socket, "DATA\r\n");

    // This is the last response code we look for until the end of the message.
    $ret = server_parse($socket, "354");
    if ($ret) {
	return $ret;
    }

    // Send the Subject Line...
    fputs($socket, "Subject: $subject\r\n");

    // Now the To Header.
    fputs($socket, "To: $to\r\n");

    // Now any custom headers....
    if (isset($headers)) {
	fputs($socket, "$headers\r\n");
    }

    // Ok now we are ready for the message...
    fputs($socket, "\r\n$body\r\n");

    // Ok the all the ingredients are mixed in let's cook this puppy...
    fputs($socket, ".\r\n");
    $ret = server_parse($socket, "250");
    if ($ret) {
	return $ret;
    }

    // Now tell the server we are done and close the socket...
    fputs($socket, "QUIT\r\n");
    fclose($socket);

    return null;
}
?>
