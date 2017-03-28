<?php
if (!defined('G2_SUPPORT')) { return; }
ob_start();
phpinfo();
$phpinfo = ob_get_contents();
ob_end_clean();

preg_match('#<body>(.*)</body>#ims', $phpinfo, $matches);
$phpinfo = $matches[1];
$phpinfo = preg_replace_callback(
    '#(<td class="v">)(.*?)(</td>)#ims',
    create_function('$matches', 'return $matches[1] . wordwrap($matches[2], 10, "<wbr>", true) . $matches[3];'),
    $phpinfo);

?>
<html>
  <head>
    <title> Gallery Support | PHP Info</title>
    <link rel="stylesheet" type="text/css" href="<?php print $baseUrl ?>support.css"/>
    <style type="text/css">
      pre {
	margin: 0px;
	font-family: monospace;
      }
      table {
	border-collapse: collapse;
	margin-left: 20px;
	width: 760 px;
      }
      td, th {
	border: 1px solid #000000;
	font-size: .9em;
	vertical-align: baseline;
      }
      .p {
	text-align: left;
      }
      .e {
	background-color: #ccccff;
	font-weight: bold;
	color: #000000;
      }
      .h {
	background-color: #9999cc;
	font-weight: bold;
	color: #000000;
      }
      .v {
	background-color: #cccccc;
	color: #000000;
      }
      i {
	color: #666666;
	background-color: #cccccc;
      }
      hr {
	background-color: #cccccc;
	height: 1px;
      }
    </style>
  </head>

  <body>
    <div id="content">
      <div id="title">
	<a href="../../">Gallery</a> &raquo;
	<a href="<?php generateUrl('index.php') ?>">Support</a> &raquo; PHP Info
      </div>
      <?php print $phpinfo; ?>
    </div>
  </body>
</html>
